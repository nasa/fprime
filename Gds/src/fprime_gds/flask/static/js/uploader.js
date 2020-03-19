
export class Uploader {
    constructor(loader) {
        this.endpoint = "/upload/files"
        this.uploading = [];
        this.loader = loader
    }

    upload(files, destination, onprogress) {
        return new Promise((success, error) => {
            this.loader.load("/upload/destination", "PUT", {"destination": destination}).then( () => {
                let data = new FormData();
                for (let i = 0; i < files.length; i++) {
                    let file = files[i];
                    data.append(file.name, file);
                }
                let xhr = new XMLHttpRequest();
                xhr.open("POST", this.endpoint, true);
                // Assign onprogress
                if (typeof (onprogress) !== "undefined") {
                    function percent(event) {
                        let percent = Math.floor(event.loaded / event.total * 100.0);
                        onprogress(percent);
                    }

                    xhr.onprogress = percent;
                }
                xhr.onload = success;
                xhr.onerror = error;
                xhr.send(data);
            });
        });
    }
};