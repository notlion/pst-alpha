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
      importScripts('${monaco_vs_path}/base/worker/workerMain.js');`
    )}`;
  }
};

const resizeEditor = () => {
  if (window.editor) window.editor.layout();
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

  let defaultSimulationShaderSource = "";
  const rendererElem = document.getElementById("renderer");
  if (rendererElem.isReady) {
    defaultSimulationShaderSource = rendererElem.getSimulationShaderSource();
  }
  else {
    rendererElem.addEventListener("ready", () => {
      defaultSimulationShaderSource = rendererElem.getSimulationShaderSource();
      if (window.editor) {
        window.editor.setValue(defaultSimulationShaderSource);
      }
    });
  }

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
    window.editor = monaco.editor.create(paneLeftElem, {
      value: defaultSimulationShaderSource,
      theme: "vs-dark",
      fontFamily: "Hack",
      language: "c",
      dragAndDrop: false,
      folding: false,
      scrollBeyondLastLine: false,
      minimap: { enabled: false },
    });

    window.editor.getModel().updateOptions({
      tabSize: 2,
    });

    window.editor.addAction({
      id: "shader-run",
      label: "Run Shader",
      keybindings: [monaco.KeyMod.Alt | monaco.KeyCode.Enter, monaco.KeyMod.CtrlCmd | monaco.KeyCode.KEY_S],
      contextMenuGroupId: "shader",
      contextMenuOrder: 0,
      run: (editor) => {
        rendererElem.setSimulationShaderSource(editor.getValue());
      }
    });

    window.editor.onDidChangeModelContent((event) => {
      // setSimulationShaderSource(window.editor.getValue());
    });

    resizeEditor();
  });

  updateLayout();
};

window.addEventListener("load", init);
window.addEventListener("resize", updateLayout);
