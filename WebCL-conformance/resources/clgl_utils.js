/*
   <!--
   Copyright (C) 2014 Samsung Electronics Corporation. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided the following conditions
   are met:

   1.  Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   2.  Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY SAMSUNG ELECTRONICS CORPORATION AND ITS
   CONTRIBUTORS "AS IS", AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING
   BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SAMSUNG
   ELECTRONICS CORPORATION OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING
   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS, OR BUSINESS INTERRUPTION), HOWEVER CAUSED AND ON ANY THEORY
   OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING
   NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   -->
*/

var WebCLGLTestUtils = (function() {

var getGLContext = function(webCLContext) {
    try {
        var webGLRenderingContext = eval("webCLContext.getGLContext();");
        if (webGLRenderingContext instanceof WebGLRenderingContext)
            return webGLRenderingContext;
    } catch (e) {
        e.description = "WebCLContext :: getGLContext threw exception : " + e.name;
        throw e;
    }
}

var createFromGLBuffer = function(webCLContext, flag, glBuffer) {
    try {
        var webCLBuffer = eval("webCLContext.createFromGLBuffer(flag, glBuffer);");
        if (webCLBuffer instanceof WebCLBuffer)
            return webCLBuffer;
    } catch (e) {
        e.description = "WebCLContext :: createFromGLBuffer threw exception : " + e.name;
        throw e;
    }
}

var createFromGLRenderbuffer = function(webCLContext, flag, glRenderbuffer) {
    try {
        var webCLImage = eval("webCLContext.createFromGLRenderbuffer(flag, glRenderbuffer);");
        if (webCLImage instanceof WebCLImage)
            return webCLImage;
    } catch (e) {
        e.description = "WebCLContext :: createFromGLRenderbuffer threw exception : " + e.name;
        throw e;
    }
}

var createFromGLTexture = function(webCLContext, flag, textureTarget, miplevel, glTexture) {
    try {
        var webCLImage = eval("webCLContext.createFromGLTexture(flag, textureTarget, miplevel, glTexture);");
        if (webCLImage instanceof WebCLImage)
            return webCLImage;
    } catch (e) {
        e.description = "WebCLContext :: createFromGLTexture threw exception : " + e.name;
        throw e;
    }
}

var enqueueAcquireGLObjects = function(webCLGLCommandQueue, memObjects, eventWaitList, webCLEvent) {
    try {
        if (typeof(webCLEvent) != 'undefined')
            webCLGLCommandQueue.enqueueAcquireGLObjects(memObjects, eventWaitList, webCLEvent);
        else if (typeof(eventWaitList) != 'undefined')
            webCLGLCommandQueue.enqueueAcquireGLObjects(memObjects, eventWaitList);
        else
            webCLGLCommandQueue.enqueueAcquireGLObjects(memObjects);
    } catch(e) {
        e.description = "WebCLGLCommandQueue :: enqueueAcquireGLObjects threw exception : " + e.name;
        throw e;
    }
}

var enqueueReleaseGLObjects = function(webCLGLCommandQueue, memObjects, eventWaitList, webCLEvent) {
    try {
        if (typeof(webCLEvent) != 'undefined')
            webCLGLCommandQueue.enqueueReleaseGLObjects(memObjects, eventWaitList, webCLEvent);
        else if (typeof(eventWaitList) != 'undefined')
            webCLGLCommandQueue.enqueueReleaseGLObjects(memObjects, eventWaitList);
        else
            webCLGLCommandQueue.enqueueReleaseGLObjects(memObjects);
    } catch(e) {
        e.description = "WebCLGLCommandQueue :: enqueueReleaseGLObjects threw exception : " + e.name;
        throw e;
    }
}

var createBuffer = function(glContext) {
    try {
        var glBuffer = eval("glContext.createBuffer();");
        if (glBuffer instanceof WebGLBuffer)
            return glBuffer;
    } catch (e) {
        e.description = "WebGLContext :: createBuffer threw exception : " + e.name;
        throw e;
    }
}

var bindBuffer = function(glContext, target, glBuffer) {
    try {
        glContext.bindBuffer(target, glBuffer);
    } catch (e) {
        e.description = "WebGLContext :: bindBuffer threw exception : " + e.name;
        throw e;
    }
}

var createRenderbuffer = function(glContext) {
    try {
        var glRenderbuffer = eval("glContext.createRenderbuffer();");
        if (glRenderbuffer instanceof WebGLRenderbuffer)
            return glRenderbuffer;
    } catch (e) {
        e.description = "WebGLContext :: createRenderbuffer threw exception : " + e.name;
        throw e;
    }
}

var bindRenderbuffer = function(glContext, target, glRenderbuffer) {
    try {
        glContext.bindRenderbuffer(target, glRenderbuffer);
    } catch (e) {
        e.description = "WebGLContext :: bindRenderbuffer threw exception : " + e.name;
        throw e;
    }
}

var createTexture = function(glContext) {
    try {
        var glTexture = eval("glContext.createTexture();");
        if (glTexture instanceof WebGLTexture)
            return glTexture;
    } catch (e) {
        e.description = "WebGLContext :: createTexture threw exception : " + e.name;
        throw e;
    }
}

var bindTexture = function(glContext, target, glTexture) {
    try {
        glContext.bindTexture(target, glTexture);
    } catch (e) {
        e.description = "WebGLContext :: bindTexture threw exception : " + e.name;
        throw e;
    }
}

return {
getGLContext:getGLContext,
createFromGLBuffer:createFromGLBuffer,
createFromGLRenderbuffer:createFromGLRenderbuffer,
createFromGLTexture:createFromGLTexture,
enqueueAcquireGLObjects:enqueueAcquireGLObjects,
enqueueReleaseGLObjects:enqueueReleaseGLObjects,
createBuffer:createBuffer,
bindBuffer:bindBuffer,
createRenderbuffer:createRenderbuffer,
bindRenderbuffer:bindRenderbuffer,
createTexture:createTexture,
bindTexture:bindTexture,
none:false
};
}());
