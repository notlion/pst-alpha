import * as mat4 from "./lib/gl-matrix-d6156a5/mat4.js";

const monaco_vs_path = 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.17.0/min/vs';
require.config({ paths: { 'vs': monaco_vs_path } });

window.MonacoEnvironment = {
  getWorkerUrl: (workerId, label) => {
    return `data:text/javascript;charset=utf-8,${encodeURIComponent(`
      self.MonacoEnvironment = {
        baseUrl: '${monaco_vs_path}'
      };
      importScripts('${monaco_vs_path}/base/worker/workerMain.js');`
    )}`;
  }
};

let canvasAspectRatio = 1;

const resizeCanvas = () => {
  const canvasContainerElem = document.getElementById("canvas-container");
  const canvasElem = document.getElementById("canvas");
  canvasElem.width = canvasContainerElem.clientWidth;
  canvasElem.height = canvasContainerElem.clientHeight;
  canvasAspectRatio = canvasElem.width / canvasElem.height;
};

const resizeEditor = () => {
  if (window.editor) window.editor.layout();
};

const updateLayout = () => {
  resizeCanvas();
  resizeEditor();
};

const getShaderSource = () => {
  return Module.UTF8ToString(Module._getShaderSource());
};
const setShaderSource = (src) => {
  const offset = Module.allocateUTF8(src);
  Module._setShaderSource(offset);
  Module._free(offset);
};
const setViewMatrix = (viewMatrixF32) => {
  const offset = Module._malloc(16 * Float32Array.BYTES_PER_ELEMENT);
  Module.HEAPF32.set(viewMatrixF32, offset / Float32Array.BYTES_PER_ELEMENT);
  Module._setViewMatrix(offset);
  Module._free(offset);
};
const setProjectionMatrix = (projectionMatrixF32) => {
  const offset = Module._malloc(16 * Float32Array.BYTES_PER_ELEMENT);
  Module.HEAPF32.set(projectionMatrixF32, offset / Float32Array.BYTES_PER_ELEMENT);
  Module._setProjectionMatrix(offset);
  Module._free(offset);
};

let rendererPrevFrameTimeMillis = 0;
let rendererTimeMillis = 0;
let rendererTimeIsPaused = false;

const initRenderer = () => {
  const idStringPtr = Module.allocateUTF8("canvas");
  Module._init(idStringPtr);
  Module._free(idStringPtr);

  const onFrame = (timestamp) => {
    const deltaTime = rendererPrevFrameTimeMillis === 0 ? 0 : timestamp - rendererPrevFrameTimeMillis;
    rendererPrevFrameTimeMillis = timestamp;

    if (!rendererTimeIsPaused) {
      rendererTimeMillis += deltaTime;
    }

    setViewMatrix(mat4.lookAt(mat4.create(), [0, 0, 3], [0, 0, 0], [0, 1, 0]));
    const DEG_TO_RAD = Math.PI / 180;
    setProjectionMatrix(mat4.perspective(mat4.create(), 60.0 * DEG_TO_RAD, canvasAspectRatio, 0.01, 1000));

    Module._update(rendererTimeMillis / 1000.0);
    Module._render();
    window.requestAnimationFrame(onFrame);
  };

  window.requestAnimationFrame(onFrame);
};

const init = () => {
  const paneLeftElem = document.getElementById("pane-left");
  const paneRightElem = document.getElementById("pane-right");

  const clamp = (x, min, max) => x < min ? min : x > max ? max : x;

  const setEditorWidthFraction = (widthFraction) => {
    const widthPercent = clamp(100 * widthFraction, 10, 90);
    paneLeftElem.style.right = (100 - widthPercent) + "%";
    paneRightElem.style.left = widthPercent + "%";
    window.localStorage.setItem("editorWidthFraction", widthFraction);
  };
  const initialEditorWidthFraction = window.localStorage.getItem("editorWidthFraction");
  if (initialEditorWidthFraction) {
    setEditorWidthFraction(initialEditorWidthFraction);
  }

  const resizeHandleElem = document.getElementById("pane-resize-handle");
  let resizeHandleDragOffsetX = 0;
  let resizeHandleDragStartEditorWidth = 0;
  let resizeHandlePrevClickTimeMillis = -1;
  let resizeHandleHasMovedSinceLastClick = false;
  const onMouseDown = (event) => {
    event.preventDefault();
    resizeHandleDragOffsetX = resizeHandleElem.offsetLeft - event.clientX;
    resizeHandleDragStartEditorWidth = paneLeftElem.clientWidth;
    document.addEventListener("mousemove", onMouseMove);
    document.addEventListener("mouseup", onMouseUp);
  };
  const onMouseUp = (event) => {
    document.removeEventListener("mousemove", onMouseMove);
    document.removeEventListener("mouseup", onMouseUp);
  }
  const onMouseMove = (event) => {
    resizeHandleHasMovedSinceLastClick = true;
    const handleX = event.clientX + resizeHandleDragOffsetX;
    setEditorWidthFraction(handleX / document.body.clientWidth);
    updateLayout();
  };
  const onClick = (event) => {
    if (!resizeHandleHasMovedSinceLastClick && resizeHandlePrevClickTimeMillis >= 0 && event.timeStamp - resizeHandlePrevClickTimeMillis < 300) {
      setEditorWidthFraction(0.5);
      updateLayout();
      resizeHandlePrevClickTimeMillis = -1;
    }
    else if (resizeHandleHasMovedSinceLastClick) {
      resizeHandlePrevClickTimeMillis = -1;
      resizeHandleHasMovedSinceLastClick = false;
    }
    else {
      resizeHandlePrevClickTimeMillis = event.timeStamp;
    }
  };
  resizeHandleElem.addEventListener("mousedown", onMouseDown);
  resizeHandleElem.addEventListener("click", onClick);

  document.getElementById("rewind-button").addEventListener("click", (event) => {
    rendererPrevFrameTimeMillis = 0;
    rendererTimeMillis = 0;
  });
  document.getElementById("play-pause-button").addEventListener("click", (event) => {
    rendererTimeIsPaused = !rendererTimeIsPaused;
  });

  require(["vs/editor/editor.main"], () => {
    window.editor = monaco.editor.create(paneLeftElem, {
      value: getShaderSource(),
      theme: "vs-dark",
      fontFamily: "Hack",
      language: "c",
      dragAndDrop: false,
      folding: false,
      scrollBeyondLastLine: false,
      minimap: { enabled: false },
    });

    window.editor.addAction({
      id: "shader-run",
      label: "Run Shader",
      keybindings: [monaco.KeyMod.Alt | monaco.KeyCode.Enter, monaco.KeyMod.CtrlCmd | monaco.KeyCode.KEY_S],
      contextMenuGroupId: "shader",
      contextMenuOrder: 0,
      run: (editor) => {
        setShaderSource(editor.getValue());
      }
    });

    window.editor.onDidChangeModelContent((event) => {
      // setShaderSource(window.editor.getValue());
    });

    resizeEditor();
  });

  updateLayout();
};

Module.onRuntimeInitialized = initRenderer;

window.addEventListener("load", init);
window.addEventListener("resize", updateLayout);
