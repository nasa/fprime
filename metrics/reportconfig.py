import io
try:
    import configparser
except ImportError:
    # python 2
    import ConfigParser as configparser


class ReportConfiguration:
    """

    """
    # TODO support multi-field comments better. start of line works, but per element might be interesting.

    def __init__(self, config_file, git_api_key=None, zen_api_key=None, section="DEFAULT"):
        # load pipeline weights and other bits, return a useful dictionary of values
        options = configparser.RawConfigParser()
        options.read(config_file)

        # get the github options for issue data retrieval
        try:
            self.git_base_url = options.get(section, "github_base_url")
        except configparser.NoOptionError as err:
            self.git_base_url = None
        if git_api_key is None:
            try:
                self.git_api_key = options.get(section, "github_api_key")
            except configparser.NoOptionError as err:
                self.git_api_key = None
        else:
            self.git_api_key = git_api_key
        try:
            self.git_repo_list = parse_entry_list(options.get(section, "github_repo_list"))
        except configparser.NoOptionError as err:
            self.git_repo_list = []
        try:
            self.git_branch = options.get(section, "github_branch")
        except configparser.NoOptionError as err:
            self.git_branch = "master"
        try:
            self.git_params_list = parse_entry_list(options.get(section, "github_issue_parameters"))
        except configparser.NoOptionError as err:
            self.git_params_list = []

        # get the zenhub options for issue data retrieval
        try:
            self.zen_base_url = options.get(section, "zenhub_base_url")
        except configparser.NoOptionError as err:
            self.zen_base_url = None
        if zen_api_key is None:
            try:
                self.zen_api_key = options.get(section, "zenhub_api_key")
            except configparser.NoOptionError as err:
                self.zen_api_key = None
        else:
            self.zen_api_key = zen_api_key

        # get issue labels for filtering against. default to including all of them and rejecting none.
        try:
            self.included_labels = parse_entry_list(options.get(section, "included_labels"))
        except configparser.NoOptionError as err:
            self.included_labels = ['*']
        try:
            self.excluded_labels = parse_entry_list(options.get(section, "excluded_labels"))
        except configparser.NoOptionError as err:
            self.excluded_labels = []

        # get issue milestones for filtering against. default to including all of them and rejecting none.
        try:
            self.included_milestones = parse_entry_list(options.get(section, "included_milestones"))
        except configparser.NoOptionError as err:
            self.included_milestones = ['*']
        try:
            self.excluded_milestones = parse_entry_list(options.get(section, "excluded_milestones"))
        except configparser.NoOptionError as err:
            self.excluded_milestones = []

        # if github issue labels have been updated, retrieve mappings
        try:
            self.git_label_mappings = {}
            for mapping in parse_entry_list(options.get(section, "github_label_mappings")):
                entry = mapping.split(":")
                self.git_label_mappings[entry[0].strip()] = entry[1].strip()
        except configparser.NoOptionError as err:
            self.git_label_mappings = {}

        # get the plan file(s)
        try:
            self.metrics_ev_plan_files = parse_entry_list(options.get(section, "metrics_ev_plan_files"))
        except configparser.NoOptionError as err:
            self.metrics_ev_plan_files = []

        # get the optional reporting output directory prefix.
        try:
            self.metrics_report_dir = options.get(section, "metrics_report_dir")
        except configparser.NoOptionError as err:
            self.metrics_report_dir = ""

        # get the storage location for metrics report artifacts
        # store them in execution dir if no alternative is provided
        try:
            self.metrics_report_artifact_dir = options.get(section, "metrics_report_artifact_dir")
        except configparser.NoOptionError as err:
            self.metrics_report_artifact_dir = "image/"

        # get the metrics report filename and sections, for auto-generated reports
        try:
            self.metrics_report_filename = options.get(section, "metrics_report_filename")
        except configparser.NoOptionError as err:
            self.metrics_report_filename = "README.md"
        try:
            self.metrics_report_sections = parse_entry_list(options.get(section, "metrics_report_sections"))
        except configparser.NoOptionError as err:
            print("WARNING: No report sections specified, no visualizations will be generated.")
            self.metrics_report_sections = []

        # get the periods for the progression tables, including issues
        try:
            temp = parse_entry_list(options.get(section, "metrics_periods"))
            self.metrics_periods = [int(x) for x in temp]
        except configparser.NoOptionError as err:
            self.metrics_periods = [60, 30, 14, 7]

        # get the metrics issue labels
        # TODO re-evaluate how these are used to filter in the metrics reports
        try:
            self.metrics_issue_labels = parse_entry_list(options.get(section, "metrics_issue_labels"))
        except configparser.NoOptionError as err:
            self.metrics_issue_labels = ['*']

        # get options for reading make targets from remote repo
        # don't do any processing here, since it's in git still
        try:
            self.metrics_make_location = options.get(section, "metrics_make_location")
        except configparser.NoOptionError as err:
            self.metrics_make_location = 'mk/configs/modules/modules.mk'

        try:
            self.metrics_make_deployment = options.get(section, "metrics_make_deployment")
        except configparser.NoOptionError as err:
            self.metrics_make_deployment = None

        try:
            self.metrics_component_report_location = options.get(section, "metrics_component_report_location")
        except configparser.NoOptionError as err:
            self.metrics_component_report_location = None

        # get the list of components to run metrics on.
        try:
            comp_list = parse_entry_list(options.get(section, "metrics_components_list"))
            self.dir_comp_map = {}
            self.components = set()
            for comp in comp_list:
                if comp == '':
                    continue
                comp = comp.strip()
                if ":" in comp:
                    entry = comp.split(":")
                    self.dir_comp_map[entry[1].strip()] = entry[0].strip()
                    self.components.add(entry[0].strip())
                else:
                    self.dir_comp_map[comp] = comp
                    self.components.add(comp)
        except configparser.NoOptionError as err:
            self.dir_comp_map = {}
            self.components = set()

        # get the optional build target prefix.
        try:
            self.metrics_build_prefix = options.get(section, "metrics_build_prefix")
        except configparser.NoOptionError as err:
            self.metrics_build_prefix = ""

        # get optional list of sloc report types
        try:
            self.metrics_sloc_types = parse_entry_list(options.get(section, "metrics_sloc_types"))
        except configparser.NoOptionError as err:
            self.metrics_sloc_types = None
        # get the file with component sloc history
        try:
            self.metrics_sloc_history = options.get(section, "metrics_sloc_history")
        except configparser.NoOptionError as err:
            self.metrics_sloc_history = None
        # get the file with component sloc history
        try:
            self.metrics_sloc_estimation = int(options.get(section, "metrics_sloc_estimation"))
        except configparser.NoOptionError as err:
            self.metrics_sloc_estimation = None

        # get optional list of comp report types
        try:
            self.metrics_comp_types = parse_entry_list(options.get(section, "metrics_comp_types"))
        except configparser.NoOptionError as err:
            self.metrics_comp_types = None
        # get the file with component structure history
        try:
            self.metrics_comp_history = options.get(section, "metrics_comp_history")
        except configparser.NoOptionError as err:
            self.metrics_comp_history = None

        # legacy options that need to be refactored out
        self.force_remote_history = False


def parse_entry_list(entry):
    return [x.replace("\\", "").strip() for x in entry.split(",")]


if __name__ == '__main__':
    raise NotImplementedError()
