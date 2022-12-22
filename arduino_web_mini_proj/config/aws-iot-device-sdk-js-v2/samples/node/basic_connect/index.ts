/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

import { mqtt } from 'aws-iot-device-sdk-v2';
import { iot } from 'aws-iot-device-sdk-v2';
import { http } from 'aws-iot-device-sdk-v2';

type Args = { [index: string]: any };

const yargs = require('yargs');

// The relative path is '../../util/cli_args' from here, but the compiled javascript file gets put one level
// deeper inside the 'dist' folder
const common_args = require('../../../util/cli_args');

yargs.command('*', false, (yargs: any) => {
    yargs.usage("Connect using certificate and private key files.");
    common_args.add_universal_arguments(yargs);
    common_args.add_common_mqtt_arguments(yargs);
    common_args.add_direct_tls_connect_arguments(yargs, true);
    common_args.add_proxy_arguments(yargs);
}, main).parse();

// Creates and returns a MQTT connection using a certificate file and key file
function build_connection(argv: Args): mqtt.MqttClientConnection {
    let config_builder = iot.AwsIotMqttConnectionConfigBuilder.new_mtls_builder_from_path(argv.cert, argv.key);

    if (argv.proxy_host) {
        config_builder.with_http_proxy_options(new http.HttpProxyOptions(argv.proxy_host, argv.proxy_port));
    }
    if (argv.ca_file != null) {
        config_builder.with_certificate_authority_from_path(undefined, argv.ca_file);
    }

    config_builder.with_clean_session(false);
    config_builder.with_client_id(argv.client_id || "test-" + Math.floor(Math.random() * 100000000));
    config_builder.with_endpoint(argv.endpoint);
    const config = config_builder.build();

    const client = new mqtt.MqttClient();
    return client.new_connection(config);
}

async function main(argv: Args) {
    common_args.apply_sample_arguments(argv);
    const connection = build_connection(argv);

    // force node to wait 20 seconds before killing itself, promises do not keep node alive
    // ToDo: we can get rid of this but it requires a refactor of the native connection binding that includes
    //    pinning the libuv event loop while the connection is active or potentially active.
    const timer = setInterval(() => { }, 20 * 1000);

    console.log("Connecting...");
    await connection.connect()
    console.log("Connection completed.");
    console.log("Disconnecting...");
    await connection.disconnect()
    console.log("Disconnect completed.");

    // Allow node to die if the promise above resolved
    clearTimeout(timer);
}
