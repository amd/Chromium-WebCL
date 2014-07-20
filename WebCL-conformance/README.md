Welcome to the WebCL Conformance Test Suite
===========================================

Note: Before adding a new test or editing an existing test
[please read these guidelines](test-guidelines.md).

This is the initial release of the WebCL conformance test suite.

Usage Notes:
------------

    Selecting platform:

                User can choose one of the various available platform, at a time, from a dropdown list.
                If only 1 platform is available, that is chosen by default and dropdown list is disabled.

    Selecting device(s):

                On selecting a platform, all the devices on the platform are displayed. User can select more
                than one device and the test suite will run on those devices.
                By default, DEFAULT device will be chosen.

    Mode of operation:
                2 modes of operations are available: strict and relax.
                In strict mode, test cases expect a particular exception but
                in relax mode, test cases expect an exception.
                By default, relax mode will be selected.

    Users can at any point of time (provided any html file is not running)
    change any of these and then submit it using `submit` button.
    Indices of selected platform and device(s) will be saved in resources/cl_global.js

    Users may wish to run a file in particular in separate tab. In such a case, test case will run on
    0th platform and on `DEFAULT` device type. And the same will be displayed on top right corner of page.

    Users may wish to run only selected file(s)/folder(s) only. The checkbox of corresponding test file
    must be unchecked to skip that particular file from running. Unchecking a checkbox at folder level,
    will skip running of all files in that folder and recursively all files of all its sub-folders.

    At any point of time, user can opt to view text summary of results of tests run so far by clicking
    `display text summary` with failed test case numbers and can even view table consisting of data of
    pass, fail, skip status of each of the run file will be available for the user. And can toggle to
    running mode and also view html summary by clicking at the then visible button `display html summary`.
