import {ParticleRendererElement} from "./renderer.js"

customElements.define("particle-renderer", ParticleRendererElement);

const monaco_vs_path = 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.17.0/min/vs';
require.config({ paths: { 'vs': monaco_vs_path } });

window.MonacoEnvironment = {
  getWorkerUrl: (workerId, label) => {
    return `data:text/javascript;charset=utf-8,${encodeURIComponent(`
      self.MonacoEnvironment = {
        baseUrl: '${monaco_vs_path}'
      };
      importScripts('${monaco_vs_path}/base/worker/workerMain.js');`)}`;
  }
};

let shaderEditorStates = null;
let shaderEditor = null;
let rendererElem = null;
let selectedShaderSourceIndex = 0;

const initEditorShaders = () => {
  shaderEditorStates.forEach((editorState, i) => {
    editorState.model.setValue(rendererElem.getShaderSourceAtIndex(i));
  });
}

const setEditorShaderIndex = (index) => {
  if (shaderEditor) {
    const nextState = shaderEditorStates[index];
    shaderEditorStates[selectedShaderSourceIndex].viewState = shaderEditor.saveViewState();
    shaderEditor.setModel(nextState.model);
    if (nextState.viewState) {
      shaderEditor.restoreViewState(nextState.viewState);
    }
    selectedShaderSourceIndex = index;
  }
};

const updateRendererShader = () => {
  if (shaderEditorStates) {
    const shaderSource = shaderEditorStates[selectedShaderSourceIndex].model.getValue();
    rendererElem.setShaderSourceAtIndex(selectedShaderSourceIndex, shaderSource);
  }
};

const resizeEditor = () => {
  if (shaderEditor) shaderEditor.layout();
};

const updateLayout = () => {
  const rendererElem = document.getElementById("renderer");
  rendererElem.updateLayout();
  resizeEditor();
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
  };
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

  rendererElem = document.getElementById("renderer");
  if (!rendererElem.isReady) {
    rendererElem.addEventListener("ready", () => {
      if (shaderEditor) {
        initEditorShaders();
        setEditorShaderIndex(0);
      }
    });
  }

  const editorTabElems = document.querySelectorAll("#editor-tabs>button");
  const updateSelectedTab = () => {
    editorTabElems.forEach((tabElem, i) => {
      tabElem.classList.toggle("selected", i === selectedShaderSourceIndex);
    });
  };
  editorTabElems.forEach((tabElem, i) => {
    tabElem.addEventListener("click", (event) => {
      setEditorShaderIndex(i);
      updateSelectedTab();
    });
  });

  const timeTextElem = document.getElementById("time-text");
  const fpsTextElem = document.getElementById("fps-text");

  const onAnimationFrame = () => {
    window.requestAnimationFrame(onAnimationFrame);

    if (rendererElem.isReady) {
      timeTextElem.textContent = (rendererElem.timeMillis / 1000).toFixed(2).toString();
      fpsTextElem.textContent = rendererElem.module._getAverageFramesPerSecond().toFixed(2).toString();
    }
  };
  window.requestAnimationFrame(onAnimationFrame);

  document.getElementById("save-shader-button").addEventListener("click", (event) => {
    updateRendererShader();
  });
  document.getElementById("rewind-button").addEventListener("click", (event) => {
    rendererElem.rewind();
  });
  document.getElementById("play-pause-button").addEventListener("click", (event) => {
    rendererElem.timeIsPaused = !rendererElem.timeIsPaused;
  });
  document.getElementById("reset-camera-button").addEventListener("click", (event) => {
    rendererElem.resetCamera();
  });
  document.getElementById("enter-vr-button").addEventListener("click", (event) => {
    rendererElem.startVRSession();
  });

  require(["vs/editor/editor.main"], () => {
    const editorContainerElem = document.getElementById("editor-container");

    shaderEditor = monaco.editor.create(editorContainerElem, {
      theme: "vs-dark",
      fontFamily: "Hack",
      dragAndDrop: false,
      folding: false,
      scrollBeyondLastLine: false,
      minimap: { enabled: false },
    });

    shaderEditorStates = new Array(2).fill().map((_, i) => {
      const model = monaco.editor.createModel("", "c");
      model.updateOptions({
        tabSize: 2,
      });
      return { model: model, viewState: null };
    });

    shaderEditor.addAction({
      id: "shader-run",
      label: "Run Shader",
      keybindings: [monaco.KeyMod.Alt | monaco.KeyCode.Enter, monaco.KeyMod.CtrlCmd | monaco.KeyCode.KEY_S],
      contextMenuGroupId: "shader",
      contextMenuOrder: 0,
      run: editor => updateRendererShader()
    });

    resizeEditor();

    if (rendererElem.isReady) {
      initEditorShaders();
      setEditorShaderIndex(0);
    }
  });

  updateLayout();
};

window.addEventListener("load", init);
window.addEventListener("resize", updateLayout);
