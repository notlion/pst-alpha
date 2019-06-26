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
      powerPreference: "high-performance",
      xrCompatible: true,
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

    this.isReady = false;

    this.module = ParticleRenderer();
    this.module.onRuntimeInitialized = () => {
      this._webglContextHandle = this.module.GL.registerContext(this.webglContext, contextAttribs);
      
      this.module.GL.makeContextCurrent(this._webglContextHandle);
      this.module._init();

      this.isReady = true;
      this.dispatchEvent(new Event("ready"));

      const onFrame = (timestamp) => {
        this._renderFrame(timestamp);
        window.requestAnimationFrame(onFrame);
      };

      window.requestAnimationFrame(onFrame);
    };

    this.camera = new Camera();
    this._cameraMovementDirection = vec3.create();

    this._prevFrameTimeMillis = 0;
    this.timeMillis = 0;
    this.timeIsPaused = false;
  }

  _renderFrame(timestamp) {
    const deltaTime = this._prevFrameTimeMillis === 0 ? 0 : timestamp - this._prevFrameTimeMillis;
    this._prevFrameTimeMillis = timestamp;

    this.camera.translateWithLocalOrientation(this._cameraMovementDirection, 0.025);
    this.camera.updateMatrices();
    this._setViewMatrix(this.camera.viewMatrix);
    this._setProjectionMatrix(this.camera.projectionMatrix);

    this.module.GL.makeContextCurrent(this._webglContextHandle);

    if (!this.timeIsPaused) {
      this.timeMillis += deltaTime;
      this.module._update(this.timeMillis / 1000.0);
    }

    this.module._render(this.canvasElem.width, this.canvasElem.height);
  }

  _onCanvasMouseDown(event) {
    this._canvasMouseIsDown = true;

    this.canvasElem.requestPointerLock();
    this.canvasElem.addEventListener("mousemove", this._canvasMouseMoveCallback);
    document.addEventListener("mouseup", (event) => {
      this._canvasMouseIsDown = false;
      vec3.zero(this._cameraMovementDirection);

      this.canvasElem.removeEventListener("mousemove", this._canvasMouseMoveCallback);
      document.exitPointerLock();
    }, { once : true });
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
        case "q":
          this._cameraMovementDirection[1] = 1;
          break;
        case "e":
          this._cameraMovementDirection[1] = -1;
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
        case "q":
          if (this._cameraMovementDirection[1] > 0) this._cameraMovementDirection[1] = 0;
          break;
        case "e":
          if (this._cameraMovementDirection[1] < 0) this._cameraMovementDirection[1] = 0;
          break;
      }
    }
  }

  _setViewMatrix(viewMatrixF32) {
    const offset = this.module._malloc(16 * Float32Array.BYTES_PER_ELEMENT);
    this.module.HEAPF32.set(viewMatrixF32, offset / Float32Array.BYTES_PER_ELEMENT);
    this.module._setViewMatrix(offset);
    this.module._free(offset);
  }

  _setProjectionMatrix(projectionMatrixF32) {
    const offset = this.module._malloc(16 * Float32Array.BYTES_PER_ELEMENT);
    this.module.HEAPF32.set(projectionMatrixF32, offset / Float32Array.BYTES_PER_ELEMENT);
    this.module._setProjectionMatrix(offset);
    this.module._free(offset);
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
    this.canvasElem.width = this.clientWidth;
    this.canvasElem.height = this.clientHeight;
    this.camera.viewportAspectRatio = this.canvasElem.width / this.canvasElem.height;
  }

  getSimulationShaderSource() {
    return this.module.UTF8ToString(this.module._getSimulationShaderSource());
  }
  setSimulationShaderSource(src) {
    const offset = this.module.allocateUTF8(src);
    this.module._setSimulationShaderSource(offset);
    this.module._free(offset);
  }
}
