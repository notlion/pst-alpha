import {mat4,
        quat,
        vec2,
        vec3} from "./lib/gl-matrix-d6156a5/index.js";
import ParticleRenderer from "./particle-renderer.js";

const DEG_TO_RAD = Math.PI / 180;

class Camera {
  constructor() {
    this.position = vec3.create();
    this.positionPrev = vec3.create();
    this.orientation = quat.create();

    this.viewportAspectRatio = 1;
    this.fieldOfViewY = 60;
    this.clipNear = 0.01;
    this.clipFar = 1000;

    this.viewMatrix = mat4.create();
    this.projectionMatrix = mat4.create();
  }

  translateWithLocalOrientation(direction, distance) {
    const dir = vec3.transformQuat(vec3.create(), direction, quat.invert(quat.create(), this.orientation));
    vec3.normalize(dir, dir);
    vec3.scale(dir, dir, distance);
    vec3.add(this.position, this.position, dir);
  }

  updateMatrices() {
    mat4.fromTranslation(this.viewMatrix, this.position);
    mat4.mul(this.viewMatrix, mat4.fromQuat(mat4.create(), this.orientation), this.viewMatrix);
    mat4.perspective(this.projectionMatrix, this.fieldOfViewY * DEG_TO_RAD, this.viewportAspectRatio, this.clipNear, this.clipFar);
  }
}

export class ParticleRendererElement extends HTMLElement {
  constructor() {
    super();

    this.canvasElem = document.createElement("canvas");
    this.canvasElem.id = "particle-renderer-canvas";
    this.appendChild(this.canvasElem);

    const contextAttribs = {
      antialias: true,
      alpha: false,
      depth: true,
      stencil: false,
      powerPreference: "high-performance",
    };

    this.webglContext = this.canvasElem.getContext("webgl2", contextAttribs);
    if (!this.webglContext) {
      console.error("Could not create a WebGL2 context");
    }

    this.canvasElem.tabIndex = 0;
    this.canvasElem.addEventListener("mousedown", event => this._onCanvasMouseDown(event));
    this.canvasElem.addEventListener("keydown", event => this._onCanvasKeyDown(event));
    this.canvasElem.addEventListener("keyup", event => this._onCanvasKeyUp(event));
    this._canvasMouseMoveCallback = (event) => this._onCanvasMouseMove(event);
    this._canvasMouseIsDown = false;

    window.addEventListener("vrdisplaypresentchange", () => this._onVRDisplayPresentChange(), false);

    this.isReady = false;

    this.module = ParticleRenderer();
    this.module.onRuntimeInitialized = () => {
      this._webglContextHandle = this.module.GL.registerContext(this.webglContext, contextAttribs);

      this.module.GL.makeContextCurrent(this._webglContextHandle);
      this.module._init();

      this.isReady = true;
      this.dispatchEvent(new Event("ready"));

      this._startAnimation();
    };

    this.camera = new Camera();
    this._cameraMovementDirection = vec3.create();
    this._cameraRollDirection = 0;
    this._cameraRollQuat = quat.create();

    this._prevFrameTimeMillis = 0;
    this.timeMillis = 0;
    this.timeIsPaused = false;
  }

  _startAnimation() {
    const onFrame = (timestamp) => {
      if (this._animationFrameCallback === onFrame) {
        window.requestAnimationFrame(onFrame);
        this._updateFrame(timestamp);
        this._renderFrame();
      }
    };
    this._animationFrameCallback = onFrame;
    window.requestAnimationFrame(onFrame);
  }

  _startVRAnimation() {
    const onVRFrame = (timestamp) => {
      if (this._animationFrameCallback === onVRFrame) {
        this.vrDisplay.requestAnimationFrame(onVRFrame);
        this._updateFrame(timestamp);
        this._renderVRFrame();
      }
    };
    this._animationFrameCallback = onVRFrame;
    this.vrDisplay.requestAnimationFrame(onVRFrame);
  }

  _updateFrame(timestamp) {
    const deltaTime = this._prevFrameTimeMillis === 0 ? 0 : timestamp - this._prevFrameTimeMillis;
    this._prevFrameTimeMillis = timestamp;

    if (!this.vrDisplay) {
      this.camera.translateWithLocalOrientation(this._cameraMovementDirection, 0.025);
      quat.identity(this._cameraRollQuat);
      quat.rotateZ(this._cameraRollQuat, this._cameraRollQuat, this._cameraRollDirection * 0.02);
      quat.mul(this.camera.orientation, this._cameraRollQuat, this.camera.orientation);
      this.camera.updateMatrices();
      this._setViewAndProjectionMatrices(this.camera.viewMatrix, this.camera.projectionMatrix);
    }

    if (navigator.getGamepads) {
      const gamepads = navigator.getGamepads();
      const controllerLeft = Array.prototype.find.call(gamepads, gp => gp && gp.hand === "left");
      if (controllerLeft) {
        this.setControllerPoseAtIndex(0, controllerLeft.pose);
      }
      const controllerRight = Array.prototype.find.call(gamepads, gp => gp && gp.hand === "right");
      if (controllerRight) {
        this.setControllerPoseAtIndex(1, controllerRight.pose);
      }
    }

    if (!this.timeIsPaused) {
      this.module.GL.makeContextCurrent(this._webglContextHandle);

      this.timeMillis += deltaTime;
      this.module._update(this.timeMillis / 1000.0);
    }
  }

  _renderFrame() {
    this.module.GL.makeContextCurrent(this._webglContextHandle);

    const gl = this.webglContext;
    gl.viewport(0, 0, this.canvasElem.width, this.canvasElem.height);
    gl.clearColor(0, 0, 0, 1);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    this.module._render(this.canvasElem.width, this.canvasElem.height);
  }

  _renderVRFrame() {
    // console.log(this.module._getAverageFramesPerSecond());

    this.module.GL.makeContextCurrent(this._webglContextHandle);

    const width = this.canvasElem.width;
    const height = this.canvasElem.height;

    const gl = this.webglContext;
    gl.viewport(0, 0, width, height);
    gl.clearColor(0, 0, 0, 1);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    this.vrDisplay.getFrameData(this.vrFrameData);

    gl.viewport(0, 0, width * 0.5, height);
    this._setViewAndProjectionMatrices(this.vrFrameData.leftViewMatrix, this.vrFrameData.leftProjectionMatrix);
    this.module._render(width * 0.5, height);

    gl.viewport(width * 0.5, 0, width * 0.5, height);
    this._setViewAndProjectionMatrices(this.vrFrameData.rightViewMatrix, this.vrFrameData.rightProjectionMatrix);
    this.module._render(width * 0.5, height);

    this.vrDisplay.submitFrame();
  }

  _onCanvasMouseDown(event) {
    this._canvasMouseIsDown = true;

    this.canvasElem.requestPointerLock();
    this.canvasElem.addEventListener("mousemove", this._canvasMouseMoveCallback);
    document.addEventListener("mouseup", (event) => {
      this._canvasMouseIsDown = false;
      vec3.zero(this._cameraMovementDirection);
      this._cameraRollDirection = 0;

      this.canvasElem.removeEventListener("mousemove", this._canvasMouseMoveCallback);
      document.exitPointerLock();
    }, { once: true });
  }

  _onCanvasMouseMove(event) {
    const rotation = quat.fromEuler(quat.create(), event.movementY * 0.25, event.movementX * 0.25, 0);
    quat.mul(this.camera.orientation, rotation, this.camera.orientation);
    quat.normalize(this.camera.orientation, this.camera.orientation);
  }

  _onCanvasKeyDown(event) {
    if (this._canvasMouseIsDown) {
      switch (event.key) {
        case "w":
          this._cameraMovementDirection[2] = 1;
          break;
        case "s":
          this._cameraMovementDirection[2] = -1;
          break;
        case "a":
          this._cameraMovementDirection[0] = 1;
          break;
        case "d":
          this._cameraMovementDirection[0] = -1;
          break;
        case "e":
          this._cameraRollDirection = 1;
          break;
        case "q":
          this._cameraRollDirection = -1;
          break;
      }
    }
  }

  _onCanvasKeyUp(event) {
    if (this._canvasMouseIsDown) {
      switch (event.key) {
        case "w":
          if (this._cameraMovementDirection[2] > 0) this._cameraMovementDirection[2] = 0;
          break;
        case "s":
          if (this._cameraMovementDirection[2] < 0) this._cameraMovementDirection[2] = 0;
          break;
        case "a":
          if (this._cameraMovementDirection[0] > 0) this._cameraMovementDirection[0] = 0;
          break;
        case "d":
          if (this._cameraMovementDirection[0] < 0) this._cameraMovementDirection[0] = 0;
          break;
        case "e":
          if (this._cameraRollDirection > 0) this._cameraRollDirection = 0;
          break;
        case "q":
          if (this._cameraRollDirection < 0) this._cameraRollDirection = 0;
          break;
      }
    }
  }

  _setViewAndProjectionMatrices(viewMatrixF32, projectionMatrixF32) {
    const viewMatrixOffset = this.module._malloc(16 * Float32Array.BYTES_PER_ELEMENT);
    this.module.HEAPF32.set(viewMatrixF32, viewMatrixOffset / Float32Array.BYTES_PER_ELEMENT);

    const projectionMatrixOffset = this.module._malloc(16 * Float32Array.BYTES_PER_ELEMENT);
    this.module.HEAPF32.set(projectionMatrixF32, projectionMatrixOffset / Float32Array.BYTES_PER_ELEMENT);

    this.module._setViewAndProjectionMatrices(viewMatrixOffset, projectionMatrixOffset);

    this.module._free(viewMatrixOffset);
    this.module._free(projectionMatrixOffset);
  }

  rewind() {
    this._prevFrameTimeMillis = 0;
    this.timeMillis = 0;
  }

  resetCamera() {
    vec3.zero(this.camera.position);
    vec3.zero(this.camera.positionPrev);
    quat.identity(this.camera.orientation);
  }

  updateLayout() {
    if (this.vrDisplay && this.vrDisplay.isPresenting) {
      const eyeL = this.vrDisplay.getEyeParameters("left");
      const eyeR = this.vrDisplay.getEyeParameters("right");
      this.canvasElem.width = Math.max(eyeL.renderWidth, eyeR.renderWidth) * 2;
      this.canvasElem.height = Math.max(eyeL.renderHeight, eyeR.renderHeight);
    }
    else {
      this.canvasElem.width = this.clientWidth;
      this.canvasElem.height = this.clientHeight;
      this.camera.viewportAspectRatio = this.canvasElem.width / this.canvasElem.height;
    }
  }

  async startVRSession() {
    try {
      const displays = await navigator.getVRDisplays();

      if (displays.length > 0) {
        this.vrFrameData = new VRFrameData();
        this.vrDisplay = displays[0];

        await this.vrDisplay.requestPresent([{ source: this.canvasElem }]);
      }
    }
    catch (err) {
      console.error(err);
    }
  }

  async stopVRSession() {
    if (this.vrDisplay && this.vrDisplay.isPresenting) {
      await this.vrDisplay.exitPresent();
    }

    this._startAnimation();
  }

  _onVRDisplayPresentChange() {
    this.updateLayout();

    if (this.vrDisplay && this.vrDisplay.isPresenting) {
      this._startVRAnimation();
    }
    else {
      this._startAnimation();
    }
  }

  getShaderSourceAtIndex(index) {
    return this.module.UTF8ToString(this.module._getUserShaderSourceAtIndex(index));
  }

  setShaderSourceAtIndex(index, src) {
    const offset = this.module.allocateUTF8(src);
    this.module._setUserShaderSourceAtIndex(index, offset);
    this.module._free(offset);
  }

  setControllerPoseAtIndex(index, pose) {
    const positionOffset = this.module._malloc(3 * Float32Array.BYTES_PER_ELEMENT);
    if (pose.position) {
      this.module.HEAPF32.set(pose.position, positionOffset / Float32Array.BYTES_PER_ELEMENT);
    }

    const velocityOffset = this.module._malloc(3 * Float32Array.BYTES_PER_ELEMENT);
    if (pose.linearVelocity) {
      this.module.HEAPF32.set(pose.linearVelocity, velocityOffset / Float32Array.BYTES_PER_ELEMENT);
    }

    this.module._setControllerPoseAtIndex(index, positionOffset, velocityOffset);

    this.module._free(positionOffset);
    this.module._free(velocityOffset);
  }
}
