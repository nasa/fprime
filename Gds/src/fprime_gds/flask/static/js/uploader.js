
export class Uploader {
    constructor(loader) {
        this.endpoint = "/upload/files"
        this.uploading = [];
        this.loader = loader
    }

    upload(files, destination) {
        return new Promise((success, error) => {
            this.loader.load("/upload/destination", "PUT", {"destination": destination}).then( () => {
                let data = new FormData();
                while(0 < files.length) {
                    let file = files.shift();
                    data.append(file.file.name, file.file);
                }
                let xhr = new XMLHttpRequest();
                xhr.open("POST", this.endpoint, true);
                xhr.onload = success;
                xhr.onerror = error;
                xhr.send(data);
            });
        });
    }

    pause() {
        return this.loader.load("/upload/files", "PUT", {"action": "pause-all"});
    }

    unpause() {
        return this.loader.load("/upload/files", "PUT", {"action": "unpause-all"});
    }

    command(file, action) {
        return this.loader.load("/upload/files", "PUT", {"source": file, "action": action});
    }

};