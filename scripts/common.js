let fs = require('fs');
let path = require('path');
let https = require('https');
let packageJson = require('../package.json');
let { execSync, spawnSync } = require('child_process');

// Switch to root dir.
process.chdir(path.dirname(__dirname));

let target_cpu = {
    x64: 'x64',
    ia32: 'x86',
    arm: 'arm',
    arm64: 'arm64',
}[process.env.npm_config_arch || process.arch];

let build_type = process.argv.indexOf('Debug') > -1? 'Debug' : 'Release';

// Find the path of cmake.
let cmakeRoot = path.resolve('node_modules', 'cmake-binaries', 'bin2');

let cmake = {
    darwin: path.join(cmakeRoot, 'CMake.app', 'Contents', 'bin', 'cmake'),
    linux: path.join(cmakeRoot, 'bin', 'cmake'),
    win32: path.join(cmakeRoot, 'bin', 'cmake.exe'),
}[process.platform];

// Make dir and ignore error.
function mkdir(dir) {
    if (fs.existsSync(dir)) {
        return;
    } 
    mkdir(path.dirname(dir));
    fs.mkdirSync(dir);
}

function download (url, callback, log = true) {
    https.get(url, (response) => {
        if (log) {
            process.stdout.write(`Downloading ${url} `);
        }

        if (response.statusCode == 302) {
            download(response.headers.location, callback, false);
            return;
        }

        let length = 0;

        response
        .on('end', () => {
            if (length > 0) {
                process.stdout.write('.');
            }
            
            console.log(' Done');
        })
        .on('data', (chunk) => {
            length += chunk.length;

            while (length >= 1024 * 1024) {
                process.stdout.write('.');
                length %= 1024 * 1024;
            }
        });
        callback(response);
    });
}

function applyDefaultExecutionOptions (options) {
    // Print command output by default.
    if (!options.stdio) {
        options.stdio = 'inherit';
    }
    
    // Merge the custom env to global env.
    if (options.env) {
        options.env = Object.assign(options.env, process.env);
    }
}

function execSyncWrapper (command, options = {}) {
    applyDefaultExecutionOptions(options);
    return execSync(command, options);
}

function spawnSyncWrapper (exec, args, options = {}) {
    applyDefaultExecutionOptions(options);
    let code = spawnSync(exec, args, options).status;
    if (code !== 0) {
        process.exit(code);
    }
}

// Export public APIs.
module.exports = {
    target_cpu,
    cmake,
    mkdir,
    download,
    exec: execSyncWrapper,
    spawn: spawnSyncWrapper,
    build_type,
    project_name: packageJson.name,
    project_version: packageJson.version
}