// Script to setup application

const { exec } = require('child_process');

const create_dict = 'python ../Gse/bin/json_dict.py';
const create_bower = 'node ./node_modules/bower/bin/bower install'
const create_omct = 'cd node_modules/openmct && export NODE_ENV=development && npm install && cd ../../'

const total_cmd = create_dict + ' && ' + create_bower;

exec(total_cmd, (err, stdout, stderr) => {
	if (err) {
		return;
	}
	console.log(`stdout: ${stdout}`);
	console.log(`stderr: ${stderr}`);
});