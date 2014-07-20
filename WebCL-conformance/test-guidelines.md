Contributing WebCL conformance tests Guidelines
===============================================

Thank you for contributing to the WebCL conformance tests.
Please try to follow these guidelines when submitting a test.

*   All changes and/or new tests should go in the conformance/  folder

*   Please use the following License with `yyyy` at the first line to be
    replaced with year of development of that file.

These lines appears at the top of every html and js file under conformance/ and resources/

    <!--
    /*
    ** Copyright (C) `yyyy` Samsung Electronics Corporation. All rights reserved.
    ** Redistribution and use in source and binary forms, with or without
    ** modification, are permitted provided the following conditions
    ** are met:
    **
    ** 1.  Redistributions of source code must retain the above copyright
    ** notice, this list of conditions and the following disclaimer.
    **
    ** 2.  Redistributions in binary form must reproduce the above copyright
    ** notice, this list of conditions and the following disclaimer in the
    ** documentation and/or other materials provided with the distribution.
    **
    ** THIS SOFTWARE IS PROVIDED BY SAMSUNG ELECTRONICS CORPORATION AND ITS
    ** CONTRIBUTORS "AS IS", AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING
    ** BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    ** FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SAMSUNG
    ** ELECTRONICS CORPORATION OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    ** INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING
    ** BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    ** DATA, OR PROFITS, OR BUSINESS INTERRUPTION), HOWEVER CAUSED AND ON ANY THEORY
    ** OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING
    ** NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
    ** EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    */ -->

*   Please use code similar to the code in existing tests
    Ideally, copy an existing test and modify it for your new test. Try not to duplicate
    code that already exists where approriate.

*   If adding a new test edit the approriate 00_test_list.txt file
    Each folder has a 00_test_list.txt file that lists the test in that folder

*   All HTML files are required to have this included.

            resources/js-test-pre.js

    which even includes all helper functions `shouldBeXXX`

*   All HTML files are required to have this included

        resources/cl_utils.js

    in order to use utility functions.

*   use the functions in WebCLTestUtils (wtu) rather than duplicating functionality.

    All the API routed through WebCLTestUtils will evaluate the API and in case of exception,
    it stores it in `description` field and helps in tracking failure, if any.

    In particular, as much as possible, keep the WebCL code in your test specific
    to the issue being tested and try to use the helper functions to handle common setup.

    Examples:
    *   to create a WebCL context call  `WebCLTestUtils.createContext`.
        *   When called without param, it will create a WebCLContext on selected devices.
        *   When called with deviceType it will create a WebCLContext for given type of device & selected platform.

    *   When running in a new tab as a web page, the same will be called with WebCL implementation default values.
        *   use WebCLTestUtils.getPlatform and WebCLTestUtils.getDevices to retrive selected devices, rather than native webcl API's.
        *   use functions like WeCLTestUtils.createBuffer, WeCLTestUtils.createSampler etc
            to create respective WebCL Objects to have a stack trace of exceptions.

*   All HTML files must have a <meta charset="utf-8">

*   Code/Tag Order

        <div id="description"></div>
        <div id="console"></div>
        <script>
        var wtu = WebCLTestUtils;
        ...

*   Ending Tests : Tests that are short and run synchronously end with

        <script src="../../resources/js-test-post.js"></script>

*   Tests that invoke a callback function generally needs to be waited for before proceeding with
    execution, to keep track of correct number of pass/fail rate.
    So `setTimeOut` function is used to move to next test case if present test case fails to call callback or to
    add script file js-test-post.js in case if present test case is the last test and is failing to call callback.

*   The test harness requries the global variable `successfullyParsed` to be set to true.
    This usually appears at the end of a file.

        var successfullyParsed = true;

*   All files must start with `debug` statement mentioning about file and signature of API to
    be tested (if applicable)

*   Entire file must be embedded in try-catch block and details of caught failure of any API in the
    course of file will be stored in `description` field (when API are routed via resources/cl_utils.js)
    of exception and the same to be printed as a failure.

