#!/usr/bin/env node

let fs = require('fs-extra');
let { target_cpu, build_type, spawn, project_name } = require('./common');

fs.copySync('resources', `target/${build_type}/resources`);

if (process.platform === 'win32') {
    let msbuild = 'C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\MSBuild\\15.0\\Bin\\MSBuild.exe';
    let platform = target_cpu === 'x64'? 'x64' : 'Win32';
    spawn(
        msbuild,
        [`${project_name}.sln`, `/p:Configuration=${build_type}`, `/p:Platform=${platform}`],
        {cwd: 'target'}
    );
}

if (process.platform === 'darwin') {
    spawnSync(
        'xcodebuild',
        ['-configuration', build_type],
        {cwd: 'target'}
    );
}

if (process.platform === 'linux') {
    spawnSync('make', [], {cwd: `target/${build_type}`});
}
