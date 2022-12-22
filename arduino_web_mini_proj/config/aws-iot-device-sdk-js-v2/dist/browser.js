"use strict";
/*
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.auth = exports.iot = exports.io = exports.http = exports.mqtt = void 0;
/**
 * Browser entry point for AWS IoT SDK.
 * @packageDocumentation
 * @module aws-iot-device-sdk
 * @mergeTarget
 */
const browser_1 = require("aws-crt/dist.browser/browser");
Object.defineProperty(exports, "mqtt", { enumerable: true, get: function () { return browser_1.mqtt; } });
Object.defineProperty(exports, "http", { enumerable: true, get: function () { return browser_1.http; } });
Object.defineProperty(exports, "io", { enumerable: true, get: function () { return browser_1.io; } });
Object.defineProperty(exports, "iot", { enumerable: true, get: function () { return browser_1.iot; } });
Object.defineProperty(exports, "auth", { enumerable: true, get: function () { return browser_1.auth; } });
//# sourceMappingURL=browser.js.map