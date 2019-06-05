import * as mat4 from "./lib/gl-matrix-d6156a5/mat4.js";

const DEG_TO_RAD = Math.PI / 180;

export class ParticleRendererElement extends HTMLElement {
  constructor() {
    super();

    this.canvasElem = document.createElement("canvas");
    this.canvasElem.id = "canvas" + 0;
    this.appendChild(this.canvasElem);

    this.module = ParticleRenderer();
    this.module.onRuntimeInitialized = () => {
      const offset = this.module.allocateUTF8("#" + this.canvasElem.id);
      this.module._init(offset);
      this.module._free(offset);
    
      const onFrame = (timestamp) => {
        this._renderFrame(timestamp);
        window.requestAnimationFrame(onFrame);
      };
    
      window.requestAnimationFrame(onFrame);
    };;

    this.canvasAspectRatio = 1;

    this.prevFrameTimeMillis = 0;
    this.timeMillis = 0;
    this.timeIsPaused = false;
  }

  _renderFrame(timestamp) {
    const deltaTime = this.prevFrameTimeMillis === 0 ? 0 : timestamp - this.prevFrameTimeMillis;
    this.prevFrameTimeMillis = timestamp;

    if (!this.timeIsPaused) {
      this.timeMillis += deltaTime;
    }

    this.setViewMatrix(mat4.lookAt(mat4.create(), [0, 0, 3], [0, 0, 0], [0, 1, 0]));
    this.setProjectionMatrix(mat4.perspective(mat4.create(), 60.0 * DEG_TO_RAD, this.canvasAspectRatio, 0.01, 1000));

    this.module._update(this.timeMillis / 1000.0);
    this.module._render();
  }

  updateLayout() {
    this.canvasElem.width = this.clientWidth;
    this.canvasElem.height = this.clientHeight;
    this.canvasAspectRatio = this.canvasElem.width / this.canvasElem.height;  
  }

  getShaderSource() {
    return this.module.UTF8ToString(this.module._getShaderSource());
  }
  setShaderSource(src) {
    const offset = this.module.allocateUTF8(src);
    this.module._setShaderSource(offset);
    this.module._free(offset);
  }

  setViewMatrix(viewMatrixF32) {
    const offset = this.module._malloc(16 * Float32Array.BYTES_PER_ELEMENT);
    this.module.HEAPF32.set(viewMatrixF32, offset / Float32Array.BYTES_PER_ELEMENT);
    this.module._setViewMatrix(offset);
    this.module._free(offset);
  }

  setProjectionMatrix(projectionMatrixF32) {
    const offset = this.module._malloc(16 * Float32Array.BYTES_PER_ELEMENT);
    this.module.HEAPF32.set(projectionMatrixF32, offset / Float32Array.BYTES_PER_ELEMENT);
    this.module._setProjectionMatrix(offset);
    this.module._free(offset);
  }  
}
