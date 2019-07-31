/**
 * command-input:
 *
 *
 */
// Setup component for select
import "../third-party/vue-select.js"

Vue.component('v-select', VueSelect.VueSelect);


Vue.component("command-input", {
    props:["commands", "loader"],
    data: function() {return {"matching": "", "selected": {"mnemonic":"NON_SELECTED", "args":[]}, "active": false}},
    template: "#command-input-template",
    methods: {
        clearArguments: function() {
            // Clear arguments
            for (let i = 0; i < this.selected.args.length; i++) {
                this.selected.args[i].value = "";
            }
        },
        sendCommand: function() {
            let _self = this;
            _self.active = true;
            let command = this.selected;
            let values = [];
            for (let i = 0; i < this.selected.args.length; i++) {
                values.push(this.selected.args[i].value);
            }
            this.loader.load("/commands/" + command.mnemonic, "PUT",
                {"key":0xfeedcafe, "arguments": values})
                .then(function() {_self.active = false;})
                .catch(function(err) {console.error("[ERROR] Failed to send command: " + err)});
        }
    },
    computed: {
        /**
         * Calculate selected object from the given ID.
         * @return command associated from selectedId
         */
        /*selected: function() {
           return this.commands[this.selectedId];
        },*/
        commandList: function() {
            return Object.values(this.commands);
        },
        /*raw: {
            set: function(value) {
                //TODO: fix this to do better tokenizing
                let tokens = value.split(",");
                if (tokens.length == 0 || !(tokens[0] in this.commands)) {
                    return;
                }
                this.selected = this.commands[tokens[0]];
                for (let i = 1; i < tokens.length && (i - 1) < this.selected.args.length; i++) {
                    this.selected.args[i-1].value = tokens[i];
                }
            },
            get: function () {
                let ret = this.selected.mnemonic;
                for (let i = 0; i < this.selected.args.length; i++) {
                    ret += ",";
                    ret += (this.selected.args[i].value == null) ? "" : this.selected.args[i].value;
                }
                return ret;
            }
        }*/
    }
});

export class CommandView {

    constructor(elemid, commands, loader) {
        this.vue = new Vue({
            el: elemid,
            data: {
                commands: commands,
                loader: loader
            }
        });
        this.vue.$refs.input.selected = commands["CMD_NO_OP"];
    }
}