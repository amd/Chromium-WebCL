/*
   <!--
   Copyright (C) 2013 Samsung Electronics Corporation. All rights reserved.

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

var SIZE = 1024;
var BUFFER_SIZE = Float32Array.BYTES_PER_ELEMENT * SIZE;

if (!window.top.CLGlobalVariables) {
    if (window.addEventListener)
        window.addEventListener('load', loadDefault, false);
    else
        window.attachEvent('onload', loadDefault);
}

function loadDefault() {
    var wtu = WebCLTestUtils;
    var webCLPlatform = wtu.getPlatform();
    var defaultDevice = wtu.getDevices(webCLPlatform, webcl.DEVICE_TYPE_DEFAULT);

    var deviceType = defaultDevice[0].getInfo(webcl.DEVICE_TYPE);
    var type = "";
    switch (deviceType) {
        case webcl.DEVICE_TYPE_GPU :
            type = "GPU";
            break;
        case webcl.DEVICE_TYPE_CPU :
            type = "CPU";
            break;
        case webcl.DEVICE_TYPE_ACCELERATOR :
            type = "ACCELERATOR";
            break;
        default :
            type = "undefined";
            break;
    }
    printDefaultMessage("1", type);
}

function printDefaultMessage(platform, device) {
    var iDiv = document.createElement('div');
    iDiv.id = 'message';
    iDiv.setAttribute("style","position: fixed;top: 2%;right: 2%;padding: 10px;font-family: monospace;background: #CCC;border: 1px solid black;");
    document.getElementsByTagName('body')[0].appendChild(iDiv);
    iDiv.innerHTML = "Platform : " + platform + " Device : " + device;
}

var WebCLTestUtils = (function() {

var generateData = function(type, size) {
    try {
        var data = eval("new type(size)");
    } catch (e) {
        throw {name : "WebCLException", message : "Expected a typed array but got " + type + "."};
    }
    for (index = 0; index < size; index++)
        data[index] = Math.floor(Math.random() * 100);
    return data;
};

var getwebCLPlatform = function() {
    var gv = window.top.CLGlobalVariables;
    var selecedPlatformIndex = gv.getInstance().getwebCLPlatformIndex();
    var selectedPlatform = webcl.getPlatforms()[selecedPlatformIndex];
    if (selectedPlatform instanceof WebCLPlatform)
        return selectedPlatform;
    throw { name : "WebCLException", message : "getwebCLPlatform() failed."};
};

var getwebCLDevices = function(selectedPlatform) {
    var gv = window.top.CLGlobalVariables;
    var selecedDevicesIndex = gv.getInstance().getwebCLDevicesIndex();
    var devicesArray = selectedPlatform.getDevices(webcl.DEVICE_TYPE_ALL);
    var selectedDevices = [];
    for (i = 0; i < selecedDevicesIndex.length; i++)
        if (selecedDevicesIndex[i] < devicesArray.length)
            selectedDevices.push(devicesArray[selecedDevicesIndex[i]]);
    if (selectedDevices.length)
        return selectedDevices;
    throw { name : "WebCLException", message : "getwebCLDevices() failed."};
};

var createContext = function(param1, param2, param3) {
    var gv = window.top.CLGlobalVariables;
    if (arguments.length > 3)
        throw {description: "Could not create context, as the number of arguments were more than expected."};
    try {
        var webCLContext;
        if (param3 != undefined)
            webCLContext = eval("webcl.createContext(param1, param2, param3);");
        else if (param2 != undefined)
            webCLContext = eval("webcl.createContext(param1, param2);");
        else if (param1 != undefined) {
            if (param1 instanceof WebGLRenderingContext) {
                var selectedDevices = gv ? getwebCLDevices(getwebCLPlatform()) :
                    webcl.getPlatforms()[0].getDevices(webcl.DEVICE_TYPE_DEFAULT);
                webCLContext = eval("webcl.createContext(param1, selectedDevices)");
            } else
                webCLContext = eval("webcl.createContext(param1);");
        } else if (gv) {
            selectedPlatform = getwebCLPlatform();
            selectedDevices = getwebCLDevices(selectedPlatform);
            webCLContext = eval("webcl.createContext(selectedDevices)");
        } else
            webCLContext = eval("webcl.createContext()");
        if (webCLContext instanceof WebCLContext)
            return webCLContext;
    } catch(e) {
        e.description = "WebCL :: createContext threw exception : " + e.name;
        throw e;
    }
}

var createProgram = function(webCLContext, kernelSource) {
    try {
        var webCLProgram = eval("webCLContext.createProgram(kernelSource)");
        if (webCLProgram instanceof WebCLProgram)
            return webCLProgram;
    } catch(e) {
        e.description = "WebCLContext :: createProgram threw exception : " + e.name;
        throw e;
    }
}

var createCommandQueue = function(webCLContext, webCLDevice, properties) {
    var gv = window.top.CLGlobalVariables;
    try {
        var webCLCommandQueue;
        if (arguments.length > 1) {
            webCLCommandQueue = eval("webCLContext.createCommandQueue(webCLDevice, properties)");
        } else {
            if (gv) {
                var selectedPlatform = getwebCLPlatform();
                var selectedDevices = getwebCLDevices(selectedPlatform);
                var dev = selectedDevices[0];
                webCLCommandQueue = eval("webCLContext.createCommandQueue(dev)");
            } else {
                webCLCommandQueue = eval("webCLContext.createCommandQueue()");
            }
        }
        if (webCLCommandQueue instanceof WebCLCommandQueue)
            return webCLCommandQueue;
    } catch(e) {
        e.description = "WebCLContext :: createCommandQueue threw exception : " + e.name;
        throw e;
    }
}

var createUserEvent = function(webCLContext) {
    try {
        var webCLUserEvent = eval("webCLContext.createUserEvent()");
        if (webCLUserEvent instanceof WebCLUserEvent)
            return webCLUserEvent;
    } catch(e) {
        e.description = "WebCLContext :: createUserEvent threw exception : " + e.name;
        throw e;
    }
}

var createEvent = function() {
    try {
        var webCLEvent = eval("new WebCLEvent()");
        if (webCLEvent instanceof WebCLEvent)
            return webCLEvent;
    } catch(e) {
        e.description = "WebCLContext :: createEvent threw exception : " + e.name;
        throw e;
    }
}

var createKernel = function(webCLProgram, kernelName) {
    try {
        var webCLKernel = eval("webCLProgram.createKernel(kernelName)");
        if (webCLKernel instanceof WebCLKernel)
            return webCLKernel;
    } catch (e) {
        e.description = "WebCLContext :: createKernel threw exception : " + e.name;
        throw e;
    }
}

var createSampler = function(webCLContext, normalizedCoords, addressingMode, filterMode) {
    try {
        var webCLSampler = eval("webCLContext.createSampler(normalizedCoords, addressingMode, filterMode)");
        if (webCLSampler instanceof WebCLSampler)
            return webCLSampler;
    } catch (e) {
        e.description = "WebCLContext :: webCLSampler threw exception : " + e.name;
        throw e;
    }
}

var getPlatform = function() {
    var gv = window.top.CLGlobalVariables;
    try {
        if (gv)
            return getwebCLPlatform();
        else {
            var webCLPlatforms = eval("webcl.getPlatforms()");
            if (typeof(webCLPlatforms) == 'object' && webCLPlatforms.length)
                return webCLPlatforms[0];
        }
    } catch(e) {
        e.description = "WebCL :: getPlatform threw exception : " + e.name;
        throw e;
    }
}

var getDevices = function(webCLPlatform, deviceType) {
    var gv = window.top.CLGlobalVariables;
    try {
        var webCLDevices;
        if (arguments.length > 1)
            webCLDevices = eval("webCLPlatform.getDevices(deviceType)");
        else {
            if (gv) {
                var selectedPlatform = getwebCLPlatform();
                return getwebCLDevices(selectedPlatform);
            } else
                webCLDevices = eval("webCLPlatform.getDevices(webcl.DEVICE_TYPE_DEFAULT)");
        }
        if (typeof(webCLDevices) == 'object' && webCLDevices.length)
            return webCLDevices;
    } catch(e) {
        e.description = "WebCLPlatform :: getDevices threw exception : " + e.name;
        throw e;
    }
}

var getSupportedImageFormats = function(webCLContext, flag, imageWidth, imageHeight)
{
    var imageFormatsArray = eval("webCLContext.getSupportedImageFormats(flag)");
    // FIXME :: Hardcoding to 1st image type. Need to check use cases.
    if (imageFormatsArray instanceof Array && imageFormatsArray.length > 0) {
	    imageFormatsArray[0].width = imageWidth;
	    imageFormatsArray[0].height = imageHeight;
        return imageFormatsArray[0]; //return {width:imageWidth, height:imageHeight, channelOrder:imageFormatsArray[0].channelOrder, channelType:imageFormatsArray[0].channelType};
    }
    throw {name:"FAILURE", message:"WebCLContext::getSupportedImageFormats( " + flag.toString(16) + " ) failed."};
}

var build = function(webCLProgram, webCLDevices, options, callback)
{
    try {
        webCLProgram.build(webCLDevices, options, callback);
        if (webCLProgram.getBuildInfo(webCLDevices[0], webcl.PROGRAM_BUILD_STATUS) == 0)
            return true;
    } catch(e) {
        e.description = "WebCLProgram :: build threw exception : " + e.name;
        e.log = webCLProgram.getBuildInfo(webCLDevices[0], webcl.PROGRAM_BUILD_LOG);
        throw e;
    }
}

var enqueueNDRangeKernel = function(webCLCommandQueue, webCLKernel, workDim, globalWorkOffset, globalWorkSize, localWorkSize, eventWaitList, webCLEvent)
{
    try {
        if (typeof(webCLEvent) != 'undefined')
            webCLCommandQueue.enqueueNDRangeKernel(webCLKernel, workDim, globalWorkOffset, globalWorkSize, localWorkSize, eventWaitList, webCLEvent);
        else if (typeof(eventWaitList) != 'undefined')
            webCLCommandQueue.enqueueNDRangeKernel(webCLKernel, workDim, globalWorkOffset, globalWorkSize, localWorkSize, eventWaitList);
        else {
	    if (globalWorkOffset == null)
		    globalWorkOffset = [];
	    if (localWorkSize == null)
		    localWorkSize = [];
            webCLCommandQueue.enqueueNDRangeKernel(webCLKernel, workDim, globalWorkOffset, globalWorkSize, localWorkSize);
	}
    } catch(e) {
        e.description = "WebCLCommandQueue :: enqueueNDRangeKernel threw exception : " + e.name;
        throw e;
    }
}

var readKernel = function(file) {
    try {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", file, false);
        xhr.send();
        var source = xhr.responseText.replace(/\r/g, "");
        if ((xhr.status === 200 || xhr.status === 0) && xhr.readyState === 4) {
            if (source.length)
                return source;
        }
        throw {name: "Failed to read Kernel."};
    } catch(e) {
        e.description = "readKernel threw exception : " + e.name;
        throw e;
    }
}

var createBuffer = function(webCLContext, flag, bufferSize, data) {
    try {
        var webCLBuffer;
        if (arguments.length > 3)
            webCLBuffer = eval("webCLContext.createBuffer(flag, bufferSize, data);");
        else
            webCLBuffer = eval("webCLContext.createBuffer(flag, bufferSize);");
        if (webCLBuffer instanceof WebCLBuffer)
            return webCLBuffer;
    } catch (e) {
        e.description = "WebCLContext :: createBuffer threw exception : " + e.name;
        throw e;
    }
}

var release = function(webCLObject) {
    try {
        eval("webCLObject.release()");
    } catch(e) {
        e.description = webCLObject + " :: release threw exception : " + e.name;
        throw e;
    }
}

var releaseAll = function(webCLObject) {
    try {
        if (webCLObject instanceof WebCLContext || webCLObject === webcl)
            eval("webCLObject.releaseAll()");
        else
            throw { description : "releaseAll is not defined for " + webCLObject };
    } catch(e) {
        e.description = webCLObject + " :: releaseAll threw exception : " + e.name;
    }
}

var setArg = function(webCLKernel, index, value) {
    try {
        webCLKernel.setArg(index, value);
    } catch(e) {
        e.description = "WebCLKernel :: setArg threw exception : " + e.name;
        throw e;
    }
}

var createSubBuffer = function(webCLBuffer, flag, origin, size) {
    try {
        var webCLSubBuffer = webCLBuffer.createSubBuffer(flag, origin, size);
        return webCLSubBuffer;
    } catch(e) {
        e.description = "WebCLBuffer :: createSubBuffer threw exception : " + e.name;
        throw e;
    }
}

var createImage = function(webCLContext, flag, imageDescriptor, data) {
    try {
        var webCLImage;
        if (arguments.length > 3)
            webCLImage = eval("webCLContext.createImage(flag, imageDescriptor, data);");
        else
            webCLImage = eval("webCLContext.createImage(flag, imageDescriptor);");
        if (webCLImage instanceof WebCLImage)
            return webCLImage;
    } catch (e) {
        e.description = "WebCLContext :: createImage threw exception : " + e.name;
        throw e;
    }
}

var enqueueCopyBuffer = function(webCLCommandQueue, srcBuffer, dstBuffer, srcOffset, dstOffset, numBytes, eventWaitList, webCLEvent) {
    try {
        if (typeof(webCLEvent) != 'undefined')
            webCLCommandQueue.enqueueCopyBuffer(srcBuffer, dstBuffer, srcOffset, dstOffset, numBytes, eventWaitList, webCLEvent);
        else if (typeof(eventWaitList) != 'undefined')
            webCLCommandQueue.enqueueCopyBuffer(srcBuffer, dstBuffer, srcOffset, dstOffset, numBytes, eventWaitList);
        else
            webCLCommandQueue.enqueueCopyBuffer(srcBuffer, dstBuffer, srcOffset, dstOffset, numBytes);
    } catch(e) {
        e.description = "WebCLCommandQueue :: enqueueCopyBuffer threw exception : " + e.name;
        throw e;
    }
}

var enqueueReadBuffer = function(webCLCommandQueue, buffer, blockingRead, bufferOffset, numBytes, dst, eventWaitList, webCLEvent) {
    try {
        if (typeof(webCLEvent) != 'undefined')
            webCLCommandQueue.enqueueReadBuffer(buffer, blockingRead, bufferOffset, numBytes, dst, eventWaitList, webCLEvent);
        else if (typeof(eventWaitList) != 'undefined')
            webCLCommandQueue.enqueueReadBuffer(buffer, blockingRead, bufferOffset, numBytes, dst, eventWaitList);
        else
            webCLCommandQueue.enqueueReadBuffer(buffer, blockingRead, bufferOffset, numBytes, dst);
    } catch(e) {
        e.description = "WebCLCommandQueue :: enqueueReadBuffer threw exception : " + e.name;
        throw e;
    }
}

var enqueueWriteBuffer = function(webCLCommandQueue, webCLBuffer, blockingWrite, bufferOffset, numBytes, hostPtr, eventWaitList, webCLEvent) {
    try {
        if (typeof(webCLEvent) != 'undefined')
            webCLCommandQueue.enqueueWriteBuffer(webCLBuffer, blockingWrite, bufferOffset, numBytes, hostPtr, eventWaitList, webCLEvent);
        else if (typeof(eventWaitList) != 'undefined')
            webCLCommandQueue.enqueueWriteBuffer(webCLBuffer, blockingWrite, bufferOffset, numBytes, hostPtr, eventWaitList);
        else
            webCLCommandQueue.enqueueWriteBuffer(webCLBuffer, blockingWrite, bufferOffset, numBytes, hostPtr);
    } catch(e) {
        e.description = "WebCLCommandQueue :: enqueueWriteBuffer threw exception : " + e.name;
        throw e;
    }
}

var enqueueCopyBufferRect = function(webCLCommandQueue, srcBuffer, dstBuffer, srcOrigin, dstOrigin, region, srcRowPitch, srcSlicePitch, dstRowPitch, dstSlicePitch, eventWaitList, webCLEvent) {
    try {
        if (typeof(webCLEvent) != 'undefined')
            webCLCommandQueue.enqueueCopyBufferRect(srcBuffer, dstBuffer, srcOrigin, dstOrigin, region, srcRowPitch, srcSlicePitch, dstRowPitch, dstSlicePitch, eventWaitList, webCLEvent);
        else if (typeof(eventWaitList) != 'undefined')
            webCLCommandQueue.enqueueCopyBufferRect(srcBuffer, dstBuffer, srcOrigin, dstOrigin, region, srcRowPitch, srcSlicePitch, dstRowPitch, dstSlicePitch, eventWaitList);
        else
            webCLCommandQueue.enqueueCopyBufferRect(srcBuffer, dstBuffer, srcOrigin, dstOrigin, region, srcRowPitch, srcSlicePitch, dstRowPitch, dstSlicePitch);
    } catch (e) {
        e.description = "WebCLCommandQueue :: enqueueCopyBufferRect threw exception : " + e.name;
        throw e;
    }
}

var enqueueReadBufferRect = function(webCLCommandQueue, buffer, blockingRead, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, hostPtr, eventWaitList, webCLEvent) {
    try {
        if (typeof(webCLEvent) != 'undefined')
            webCLCommandQueue.enqueueReadBufferRect(buffer, blockingRead, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, hostPtr, eventWaitList, webCLEvent);
        else if (typeof(eventWaitList) != 'undefined')
            webCLCommandQueue.enqueueReadBufferRect(buffer, blockingRead, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, hostPtr, eventWaitList);
        else
            webCLCommandQueue.enqueueReadBufferRect(buffer, blockingRead, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, hostPtr);
    } catch(e) {
        e.description = "WebCLCommandQueue :: enqueueReadBufferRect threw exception : " + e.name;
        throw e;
    }
}

var enqueueWriteBufferRect = function(webCLCommandQueue, buffer, blockingWrite, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, hostPtr, eventWaitList, webCLEvent) {
    try {
        if (typeof(webCLEvent) != 'undefined')
            webCLCommandQueue.enqueueWriteBufferRect(buffer, blockingWrite, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, hostPtr, eventWaitList, webCLEvent);
        else if (typeof(eventWaitList) != 'undefined')
            webCLCommandQueue.enqueueWriteBufferRect(buffer, blockingWrite, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, hostPtr, eventWaitList);
        else
            webCLCommandQueue.enqueueWriteBufferRect(buffer, blockingWrite, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, hostPtr);
    } catch(e) {
        e.description = "WebCLCommandQueue :: enqueueWriteBufferRect threw exception : " + e.name;
        throw e;
    }
}

var enqueueCopyImage = function(webCLCommandQueue, srcImage, dstImage, srcOrigin, dstOrigin, region, eventWaitList, webCLEvent) {
    try {
        if (typeof(webCLEvent) != 'undefined')
            webCLCommandQueue.enqueueCopyImage(srcImage, dstImage, srcOrigin, dstOrigin, region, eventWaitList, webCLEvent);
        else if (typeof(eventWaitList) != 'undefined')
            webCLCommandQueue.enqueueCopyImage(srcImage, dstImage, srcOrigin, dstOrigin, region, eventWaitList);
        else
            webCLCommandQueue.enqueueCopyImage(srcImage, dstImage, srcOrigin, dstOrigin, region);
    } catch(e) {
        e.description = "WebCLCommandQueue :: enqueueCopyImage threw exception : " + e.name;
        throw e;
    }
}

var enqueueReadImage = function(webCLCommandQueue, image, blockingRead, origin, region, hostRowPitch, dst, eventWaitList, webCLEvent) {
    try {
        if (typeof(webCLEvent) != 'undefined')
            webCLCommandQueue.enqueueReadImage(image, blockingRead, origin, region, hostRowPitch, dst, eventWaitList, webCLEvent);
        else if (typeof(eventWaitList) != 'undefined')
            webCLCommandQueue.enqueueReadImage(image, blockingRead, origin, region, hostRowPitch, dst, eventWaitList);
        else
            webCLCommandQueue.enqueueReadImage(image, blockingRead, origin, region, hostRowPitch, dst);
    } catch(e) {
        e.description = "WebCLCommandQueue :: enqueueReadImage threw exception : " + e.name;
        throw e;
    }
}

var enqueueWriteImage = function(webCLCommandQueue, image, blockingWrite, origin, region, hostRowPitch, hostPtr, eventWaitList, webCLEvent) {
    try {
        if (typeof(webCLEvent) != 'undefined')
            webCLCommandQueue.enqueueWriteImage(image, blockingWrite, origin, region, hostRowPitch, hostPtr, eventWaitList, webCLEvent);
        else if (typeof(eventWaitList) != 'undefined')
            webCLCommandQueue.enqueueWriteImage(image, blockingWrite, origin, region, hostRowPitch, hostPtr, eventWaitList);
        else
            webCLCommandQueue.enqueueWriteImage(image, blockingWrite, origin, region, hostRowPitch, hostPtr);
    } catch(e) {
        e.description = "WebCLCommandQueue :: enqueueWriteImage threw exception : " + e.name;
        throw e;
    }
}

var enqueueCopyBufferToImage = function(webCLCommandQueue, srcBuffer, dstImage, srcOffset, dstOrigin, dstRegion, eventWaitList, webCLEvent) {
    try {
        if (typeof(webCLEvent) != 'undefined')
            webCLCommandQueue.enqueueCopyBufferToImage(srcBuffer, dstImage, srcOffset, dstOrigin, dstRegion, eventWaitList, webCLEvent);
        else if (typeof(eventWaitList) != 'undefined')
            webCLCommandQueue.enqueueCopyBufferToImage(srcBuffer, dstImage, srcOffset, dstOrigin, dstRegion, eventWaitList);
        else
            webCLCommandQueue.enqueueCopyBufferToImage(srcBuffer, dstImage, srcOffset, dstOrigin, dstRegion);
    } catch(e) {
        e.description = "WebCLCommandQueue :: enqueueCopyBufferToImage threw exception : " + e.name;
        throw e;
    }
}

var enqueueCopyImageToBuffer = function(webCLCommandQueue, srcImage, dstBuffer, srcOrigin, srcRegion, dstOffset, eventWaitList, webCLEvent) {
    try {
        if (typeof(webCLEvent) != 'undefined')
            webCLCommandQueue.enqueueCopyImageToBuffer(srcImage, dstBuffer, srcOrigin, srcRegion, dstOffset, eventWaitList, webCLEvent);
        else if (typeof(eventWaitList) != 'undefined')
            webCLCommandQueue.enqueueCopyImageToBuffer(srcImage, dstBuffer, srcOrigin, srcRegion, dstOffset, eventWaitList);
        else
            webCLCommandQueue.enqueueCopyImageToBuffer(srcImage, dstBuffer, srcOrigin, srcRegion, dstOffset);
    } catch (e) {
        e.description = "WebCLCommandQueue :: enqueueCopyImageToBuffer threw exception : " + e.name;
        throw e;
    }
}

var generateRandomInt = function(data, loopSize) {
    for (i = 0; i < loopSize; i++)
        data[i] = Math.floor(Math.random() * 10) + 1;
}

var generateRandomFloat = function(data, loopSize) {
    for (i = 0; i < loopSize; i++)
        data[i] = Math.random() * 10;
}

var generateRandomNumberInRange = function (data, min, max, loopSize) {
    for (i = 0; i < loopSize; i++)
        data[i] = Math.random() * (max - min) + min;
}

var verifyResult = function(data, result, loopSize, msg) {
    correct = 0;
    for (i = 0; i < loopSize; i++)
        if (data[i] == result[i])
            correct++;
    if (correct == loopSize)
        testPassed("Test passed for " + msg + ".");
    else
        testFailed("Test failed for " + msg + ". Computed " + correct + " / " + loopSize + " correct values.");
}

var getSupportedExtensions = function(webCLObject) {
    try {
        return webCLObject.getSupportedExtensions();
    } catch(e) {
        e.description = webCLObject + " :: getSupportedExtensions threw exception : " + e.name;
        throw e;
    }
}
var setStatus = function(webCLEvent, statusValue) {
    try {
        webCLEvent.setStatus(statusValue);
    } catch(e) {
        e.description = "WebCLEvent :: setStatus threw exception : " + e.name;
        throw e;
    }
}

var getArgInfo = function(webCLKernel, index) {
    try {
        return webCLKernel.getArgInfo(index);
    } catch(e) {
        e.description = "webCLKernel :: getArgInfo (" + index + ") threw exception : " + e.name;
        throw e;
    }
}

var enableExtension = function(object, extensionName) {
    try {
        return object.enableExtension(extensionName);
    } catch(e) {
        e.description = object + " :: enableExtension ( " + extensionName + " ) threw exception : " + e.name;
        throw e;
    }
}

var enqueueMarker = function(webCLCommandQueue, webCLEvent) {
    try {
        return webCLCommandQueue.enqueueMarker(webCLEvent);
    } catch(e) {
        e.description = "webCLCommandQueue :: enqueueMarker threw exception : " + e.name;
        throw e;
    }
}

var verifyArrayForZeroValues = function(array, arraySize, msg) {
    try {
        for (index = 0; index < arraySize; index++) {
            if (array[index] != 0) {
                testFailed(msg);
                return;
            }
        }
        testPassed(msg);
    } catch(e) {
        e.description = "Verifying if the array " + array + "is null, threw exception : " + e.name;
        throw e;
    }
}

var getBytesForChannelOrder = function(channelOrder) {
    switch (channelOrder) {
        case webcl.R:
        case webcl.A:
        case webcl.INTENSITY:
        case webcl.LUMINANCE:
            return 1;
        case webcl.RG:
        case webcl.RA:
        case webcl.Rx:
            return 2;
        case webcl.RGB:
        case webcl.RGx:
            return 3;
        case webcl.RGBA:
        case webcl.BGRA:
        case webcl.ARGB:
        case webcl.RGBx:
            return 4;
    }
}

var getArrayTypeForChanneltype = function(channelType) {
    switch (channelType) {
        case webcl.SNORM_INT8:
        case webcl.UNORM_INT8:
        case webcl.SIGNED_INT8:
        case webcl.UNSIGNED_INT8:
            return "Uint8Array";
        case webcl.SNORM_INT16:
        case webcl.UNORM_INT16:
        case webcl.SIGNED_INT16:
        case webcl.UNSIGNED_INT16:
        case webcl.HALF_FLOAT:
            return "Uint16Array";
        case webcl.SIGNED_INT32:
        case webcl.UNSIGNED_INT32:
        case webcl.FLOAT:
            return "Uint32Array";
        case webcl.UNORM_SHORT_565:
        case webcl.UNORM_SHORT_555:
        case webcl.UNORM_INT_101010:
            throw {description: "getArrayTypeForChanneltype threw exception as " + channelType + " is not supported"};
    }
}

var waitForEvents = function(webCLEvents) {
    try {
        return webcl.waitForEvents(webCLEvents);
    } catch (e) {
        e.description = "webCL :: waitForEvents threw exception : " + e.name;
        throw e;
    }
}

var finish = function(webCLCommandQueue, callback) {
    try {
        if (arguments.length > 1)
            webCLCommandQueue.finish(callback);
        else
            webCLCommandQueue.finish();
    } catch(e) {
        e.description = "WebCLCommandQueue :: finish threw exception : " + e.name;
        throw e;
    }
}

var setCallback = function(event, commandExecCallbackType, callback)
{
    try {
        return event.setCallback(commandExecCallbackType, callback);
    } catch(e) {
        e.description = "WebCLEvent :: setCallback threw exception : " + e.name;
        throw e;
    }
}

var appendPostJSToHTML = function(document)
{
    var script = document.createElement('script');
    script.src = '../../../resources/js-test-post.js';
    script.type = 'text/javascript';
    document.getElementsByTagName('head')[0].appendChild(script);
}

return {
createContext:createContext,
createProgram:createProgram,
createCommandQueue:createCommandQueue,
createUserEvent:createUserEvent,
createEvent:createEvent,
createKernel:createKernel,
createSampler:createSampler,
getPlatform:getPlatform,
getDevices:getDevices,
getSupportedImageFormats:getSupportedImageFormats,
build:build,
enqueueNDRangeKernel:enqueueNDRangeKernel,
readKernel:readKernel,
generateData:generateData,
createBuffer:createBuffer,
createImage:createImage,
release:release,
releaseAll:releaseAll,
setArg:setArg,
createSubBuffer:createSubBuffer,
enqueueCopyBuffer:enqueueCopyBuffer,
enqueueReadBuffer:enqueueReadBuffer,
enqueueWriteBuffer:enqueueWriteBuffer,
enqueueCopyBufferRect:enqueueCopyBufferRect,
enqueueReadBufferRect:enqueueReadBufferRect,
enqueueWriteBufferRect:enqueueWriteBufferRect,
enqueueCopyImage:enqueueCopyImage,
enqueueReadImage:enqueueReadImage,
enqueueWriteImage:enqueueWriteImage,
enqueueCopyBufferToImage:enqueueCopyBufferToImage,
enqueueCopyImageToBuffer:enqueueCopyImageToBuffer,
generateRandomInt:generateRandomInt,
generateRandomFloat:generateRandomFloat,
generateRandomNumberInRange:generateRandomNumberInRange,
verifyResult:verifyResult,
getSupportedExtensions:getSupportedExtensions,
setStatus:setStatus,
getArgInfo:getArgInfo,
enableExtension:enableExtension,
enqueueMarker:enqueueMarker,
getBytesForChannelOrder:getBytesForChannelOrder,
getArrayTypeForChanneltype:getArrayTypeForChanneltype,
verifyArrayForZeroValues:verifyArrayForZeroValues,
waitForEvents:waitForEvents,
finish:finish,
setCallback:setCallback,
appendPostJSToHTML:appendPostJSToHTML,
none:false
};
}());
