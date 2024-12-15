let App = (function() {
  let pitch = 0;
  let roll = 0;

  function keyPress(event) {
    console.log("key press: ", event);
    let updated = false;
    if (event.code == 'KeyA') {
      roll -= 0.5;
      updated = true;
    }
    if (event.code == 'KeyD') {
      roll += 0.5;
      updated = true;
    }
    if (event.code == 'KeyW') {
      pitch -= 0.25;
      updated = true;
    }
    if (event.code == 'KeyS') {
      pitch += 0.25;
      updated = true;
    }

    if (updated) {
      rollSpan.innerText = `${roll}`;
      pitchSpan.innerText = `${pitch}`;

      drawAttitudeIndicator();
      drawComposited();
    }
  }

  function init() {
    console.log("Init");

    drawBitmaps();
    drawAttitudeIndicatorSrc();
    drawAttitudeIndicator();
    drawComposited();

    window.addEventListener('keydown', keyPress);
  }

  function IDX(x, y) {
    return (y*320 + x) * 4;
  }

  function copyContext(inputCtx, outputCtx) {
    let imageData = inputCtx.getImageData(0, 0, 320, 240);
    console.log(imageData);

    outputCtx.putImageData(imageData, 0, 0);
  }

  function copyContextWithTransform(inputCtx, outputCtx, m) {
    let srcData = inputCtx.getImageData(0, 0, 320, 240);
    let src = srcData.data;

    let dst = new Uint8ClampedArray(4 * 320 * 240);
    for (let y = 0; y < 240; y++) {
      let dst_p_start = new DOMPoint(0, y);
      let src_p_start = dst_p_start.matrixTransform(m); 
      let dst_p_end = new DOMPoint(320, y);
      let src_p_end = dst_p_end.matrixTransform(m); 

      let x_start = src_p_start.x;
      let y_start = src_p_start.y;
      let x_end = src_p_end.x;
      let y_end = src_p_end.y;

      let dx = (x_end - x_start) / 320.0;
      let dy = (y_end - y_start) / 320.0;
      for (let x = 0; x < 320; x++) {
        let xs = Math.round(x_start + x * dx);
        let ys = Math.round(y_start + x * dy);
        if (xs > 0 && xs < 320 && ys > 0 && ys < 240) {
          dst[IDX(x, y) + 0] = src[IDX(xs, ys) + 0];
          dst[IDX(x, y) + 1] = src[IDX(xs, ys) + 1];
          dst[IDX(x, y) + 2] = src[IDX(xs, ys) + 2];
          dst[IDX(x, y) + 3] = src[IDX(xs, ys) + 3];
        }
      }
    }

    let dstData = new ImageData(dst, 320, 240);
    console.log(dstData);

    outputCtx.putImageData(dstData, 0, 0);
  }

  function drawAttitudeIndicatorSrc() {
    let ctx = attitudeIndicatorSrcCanvas.getContext("2d");
    let t = ctx.getTransform();
    ctx.translate(160, 120);
    ctx.translate(-160, -120);
    drawAI(ctx);
    ctx.setTransform(t);

    ctx = horizonSrcCanvas.getContext("2d");
    t = ctx.getTransform();
    ctx.translate(160, 120);
    ctx.translate(-160, -120);
    drawHorizon(ctx);
    ctx.setTransform(t);
  }

  function drawAttitudeIndicator() {
    let ctx = attitudeIndicatorCanvas.getContext("2d");
    let t = ctx.getTransform();

    /*
    ctx.translate(160, 120);
    ctx.rotate((roll * Math.PI)/180);
    ctx.translate(-160, -120);
    */

    let cos_theta = Math.cos((-roll * Math.PI)/180);
    let sin_theta = Math.sin((-roll * Math.PI)/180);
    let tx = -160;
    let ty = -120;
    let m = new DOMMatrix([
                  cos_theta, sin_theta,
                  -sin_theta, cos_theta,
                  tx * cos_theta - ty * sin_theta - tx,
                  tx * sin_theta + ty * cos_theta - ty ]);

    copyContextWithTransform(attitudeIndicatorSrcCanvas.getContext("2d"), ctx, m);

    ctx = horizonCanvas.getContext("2d");
    ctx.fillStyle = "white";
    ctx.fillRect(0, 0, 320, 240);

    /*
    cos_theta = Math.cos((roll * Math.PI)/180);
    sin_theta = Math.sin((roll * Math.PI)/180);

    t = ctx.getTransform();
    ctx.translate(160, 120);
    ctx.rotate((roll * Math.PI)/180);
    ctx.translate(-160, -120);
    ctx.transform(cos_theta, sin_theta,
                  -sin_theta, cos_theta,
                  tx * cos_theta - ty * sin_theta - tx,
                  (tx * sin_theta + ty * cos_theta - ty) + (4 * pitch));
    // ctx.translate(0, 4 * pitch);
    drawHorizon(ctx);
    //ctx.setTransform(t);
    */
    let h = new DOMMatrix([
                  cos_theta, sin_theta,
                  -sin_theta, cos_theta,
                  tx * cos_theta - ty * sin_theta - tx,
                  tx * sin_theta + ty * cos_theta - ty + (4 * pitch)]);
    copyContextWithTransform(horizonSrcCanvas.getContext("2d"), ctx, h);

  }

  function drawBitmaps() {
    let ctx = bitmaskInnerCanvas.getContext("2d");
    drawBitmaskInner(ctx);

    ctx = bitmaskOuterCanvas.getContext("2d");
    drawBitmaskOuter(ctx);
  }

  function drawAI(ctx) {
    ctx.fillStyle = "white";
    ctx.fillRect(-40, -40, 480, 320);

    ctx.fillStyle = "rgb(102,167,225)";
    ctx.fillRect(40, 0, 240, 120);

    ctx.fillStyle = "rgb(224, 147, 27)";
    ctx.fillRect(40, 120, 240, 120);

    ctx.strokeStyle = "white";
    ctx.lineWidth = 4;
    ctx.beginPath();
    ctx.arc(160, 120, 100, 0, 2 * Math.PI);
    ctx.stroke();

    function drawTick(angle, width) {
      let t = ctx.getTransform();
      ctx.translate(160, 120);
      ctx.rotate((angle * Math.PI)/180);
      ctx.translate(-160, -120);
      ctx.strokeStyle = "white";
      ctx.lineWidth = width;
      ctx.beginPath();
      ctx.moveTo(160, 3);
      ctx.lineTo(160, 18);
      ctx.stroke();
      ctx.setTransform(t);
    }
    
    drawTick(-90, 6);
    drawTick(-45, 6);
    drawTick(-30, 6);
    drawTick(-20, 4);
    drawTick(-10, 4);
    drawTick(0, 8);
    drawTick(10, 4);
    drawTick(20, 4);
    drawTick(30, 6);
    drawTick(45, 6);
    drawTick(90, 6);
  }

  function drawHorizon(ctx) {

    ctx.fillStyle = "rgb(102,167,225)";
    ctx.fillRect(40, -80, 240, 200);

    ctx.fillStyle = "rgb(224, 147, 27)";
    ctx.fillRect(40, 120, 240, 200);

    ctx.strokeStyle = "white";
    ctx.lineWidth = 6;
    ctx.beginPath();
    ctx.moveTo(0, 120);
    ctx.lineTo(320, 120);
    ctx.stroke();

    ctx.lineWidth = 4;
    ctx.beginPath();
    ctx.moveTo(145, 80);
    ctx.lineTo(175, 80);
    ctx.moveTo(140, 100);
    ctx.lineTo(180, 100);
    ctx.moveTo(140, 140);
    ctx.lineTo(180, 140);
    ctx.moveTo(145, 160);
    ctx.lineTo(175, 160);
    ctx.stroke();
  }

  function drawBitmaskInner(ctx) {
    ctx.fillStyle = "white";
    ctx.fillRect(40, 0, 240, 240);
    ctx.fillStyle = "black";
    let circle = new Path2D();
    circle.arc(160, 120, 100, 0, 2 * Math.PI);
    circle.closePath();
    ctx.fill(circle);
  }

  function drawBitmaskOuter(ctx) {
    ctx.fillStyle = "black";
    ctx.fillRect(40, 0, 240, 240);
    ctx.fillStyle = "white";
    let circle = new Path2D();
    circle.arc(160, 120, 118, 0, 2 * Math.PI);
    circle.closePath();
    ctx.fill(circle);
  }

  function drawComposited() {
    let srcCtx = attitudeIndicatorCanvas.getContext("2d");
    let srcData = srcCtx.getImageData(0, 0, 320, 240);
    let src = srcData.data;

    let maskCtx = bitmaskInnerCanvas.getContext("2d");
    let maskData = maskCtx.getImageData(0, 0, 320, 240);
    let mask = maskData.data;

    let maskOuterCtx = bitmaskOuterCanvas.getContext("2d");
    let maskOuterData = maskOuterCtx.getImageData(0, 0, 320, 240);
    let maskOuter = maskOuterData.data;

    let dst = new Uint8ClampedArray(4 * 320 * 240);
    for (let y = 0; y < 240; y++) {
      for (let x = 40; x < 280; x++) {
        dst[IDX(x, y) + 0] = src[IDX(x, y) + 0] & mask[IDX(x, y) + 0] & maskOuter[IDX(x, y) + 0];
        dst[IDX(x, y) + 1] = src[IDX(x, y) + 1] & mask[IDX(x, y) + 1] & maskOuter[IDX(x, y) + 1];
        dst[IDX(x, y) + 2] = src[IDX(x, y) + 2] & mask[IDX(x, y) + 2] & maskOuter[IDX(x, y) + 2];
        dst[IDX(x, y) + 3] = 255;
      }
    }

    srcCtx = horizonCanvas.getContext("2d");
    srcData = srcCtx.getImageData(0, 0, 320, 240);
    src = srcData.data;

    for (let y = 0; y < 240; y++) {
      for (let x = 0; x < 280; x++) {
        dst[IDX(x, y) + 0] |= src[IDX(x, y) + 0] & ~mask[IDX(x, y) + 0];
        dst[IDX(x, y) + 1] |= src[IDX(x, y) + 1] & ~mask[IDX(x, y) + 1];
        dst[IDX(x, y) + 2] |= src[IDX(x, y) + 2] & ~mask[IDX(x, y) + 2];
      }
    }

    let dstCtx = compositedCanvas.getContext("2d");
    let dstData = new ImageData(dst, 320, 240);
    dstCtx.putImageData(dstData, 0, 0);

    dstCtx.strokeStyle = "red";
    dstCtx.lineWidth = 3;
    dstCtx.beginPath();
    dstCtx.moveTo(155, 0);
    dstCtx.lineTo(160, 10);
    dstCtx.lineTo(165, 0);

    dstCtx.moveTo(120, 120);
    dstCtx.lineTo(155, 120);
    dstCtx.lineTo(160, 125);
    dstCtx.lineTo(165, 120);
    dstCtx.lineTo(200, 120);
    dstCtx.stroke();
  }

  return {
    init: init
  };
})();

if (document.readyState === "loading") {
  document.addEventListener("DOMContentLoaded", App.init);
} else {
  App.init();
}
