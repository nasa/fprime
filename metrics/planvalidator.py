import sys
import gheconnector
import datetime
import csv

import reportconfig


class PlanValidator:
    """ Object class to store one or more plans read from csv files, along with associated methods to operate on them.

    """
    # EV plan report accessors
    EV = "Earned Value"
    TASK = "Task"
    ENG = "Engineer"
    REL = "Release"
    PV = "Planned Value"
    START = "Scheduled Start"
    END = "Scheduled End"
    GATE = "Gates"
    GHE = "GitHub Issue URL"
    DIFF = "Fields that differ from GHE"

    def __init__(self, plan_file_list):
        self.primary_plan = ""
        self.plans = {}
        self.tasks = {}
        self.gates = {}
        self.releases = {}
        for index, plan_file in enumerate(plan_file_list):
            plan_key = self.generate_plan_file_key(plan_file)
            try:
                self.plans[plan_key], self.tasks[plan_key], self.gates[plan_key], self.releases[plan_key] = \
                    self._generate_plan_data(plan_file)
                if not self.primary_plan:
                    self.primary_plan = plan_key
            except IOError as err:
                print("WARN: Unable to open {}. Error: {}".format(plan_file, err.message))
                continue

    @staticmethod
    def generate_plan_file_key(plan_file):
        """ Helper method to turn a filename into a plan_key. Strips preceding directories and extensions from a plan
        file path.

        :param plan_file: Plan file string to convert to simple key.
        :return: Plan key string.
        """
        plan_key = plan_file
        if plan_key.rfind('.') > -1:
            plan_key = plan_key[:-4]
        if plan_key.rfind('/') > -1:
            plan_key = plan_key[plan_key.rfind('/') + 1:]
        return plan_key

    def _generate_plan_data(self, plan_file):
        """ Generates plan data from the indicated set of files. The first plan file in the list will be treated as
        primary and will dictate values for completed issues. Additional plan files in the list will be plotted as
        alternate plans. The name of the plots is set by the filename.

        :param plan_file: Plan to read and store in planvalidator object.
        :return: Tuple of plan dictionary and task names list
        """

        # if defined local file, get history from file
        with open(plan_file, 'rb') as file_ptr:
            comp_reader = csv.reader(file_ptr)
            data = list(comp_reader)
        # if we can't open it, warn and continue onto others

        plan = {}
        gates = {}
        releases = {}
        # tasks list is just to retain file ordering
        tasks = []

        headers = data[0]
        # fix for junk at start of file in some excel saves
        for index, value in enumerate(headers):
            headers[index] = value.replace("\xef\xbb\xbf", "")

        t = headers.index(self.TASK)
        n = headers.index(self.ENG)
        r = headers.index(self.REL)
        v = headers.index(self.PV)
        s = headers.index(self.START)
        e = headers.index(self.END)
        g = headers.index(self.GATE)
        data = data[1:]

        # generate EV info and references for each line item
        for line in data:
            try:
                start_date = datetime.datetime.strptime(line[s], "%Y-%m-%d").date()
                end_date = datetime.datetime.strptime(line[e], "%Y-%m-%d").date()
            except Exception as err:
                print("WARN: Invalid date format found in task '{}' of '{}'. Task will be skipped. Error: {}"
                      .format(line[t], plan_file, err))
                continue
            if line[t] in plan:
                raise Exception("Non-unique task name \"{}\" found in \"{}\". Aborting plan generation."
                                .format(line[t], plan_file))
            gates_line = line[g:]
            if not gates_line:
                gates_line = ["", '1.']
            if len(gates_line) % 2 != 0:
                gates_line.append('1.')

            plan[line[t]] = self._process_plan_task(line[t], line[n], line[r], float(line[v]),
                                                    start_date, end_date, gates_line)
            tasks.append(line[t])

            for gate in plan[line[t]][self.GATE].keys():
                if gate:
                    gates[gate] = {}

            releases[line[r]] = {}

        return plan, tasks, gates, releases

    def _process_plan_task(self, task, engineer, release, task_ev, start_date, end_date, gates):
        """Generates a task dict object based on the decomposed task line information.

        :param engineer: Engineer assigned for the task.
        :param release: Release for the task.
        :param task_ev: Earned value for the task across all gates.
        :param start_date: Start date for the task; used to infer start and end dates for gates.
        :param end_date: End date for the task; used to infer start and end dates for gates.
        :param gates: List of gates and percentages from the plan.
        :return: Dictionary object for the task.
        """
        # gate weights are additive rather than cumulative internally to simplify EV charting, but we
        # expect users to input them in a cumulative fashion for simplicity.

        prev_percent = 0.
        task_end = start_date
        task_dict = {self.TASK: task,
                     self.EV: task_ev,
                     self.START: start_date,
                     self.END: end_date,
                     self.REL: release,
                     self.ENG: engineer,
                     self.GATE: {}
                     }
        for index, gate in enumerate(gates):
            if index % 2 == 1:
                continue
            if not gate and index > 0:
                continue
            elif not gate and index == 0:
                gates = [""]

            if index + 1 >= len(gates):
                percent = 1.
            else:
                percent = gates[index + 1]
                percent = float(percent.strip().replace('%', ''))
                if percent > 1.:
                    percent = percent / 100.

            # contribution is task_ex time the difference between recorded_percent and the previous_percent
            contribution = task_ev * (percent - prev_percent)

            task_start = task_end
            task_end = start_date + datetime.timedelta((end_date - start_date).days * percent)

            # update prev_percent so we get the correct contribution from the next section
            prev_percent = percent

            task_dict[self.GATE][gate] = {self.START: task_start,
                                          self.END: task_end,
                                          self.EV: contribution
                                          }
        return task_dict

    def validate_github_milestones_against_plan(self, ghe_conn, repo_name, plan_key=None, show_found=False, strict=False):
        if plan_key is None:
            plan_key = self.primary_plan
        # get all of the issues
        ghe_milestones = ghe_conn.get_milestones(repo_name, events=False)
        ghe_milestone_names = []
        for ghe_milestone in ghe_milestones:
            ghe_milestone_names += ghe_milestone["title"]
        for release in self.releases[plan_key]:
            if release not in ghe_milestone_names:
                print("Unable to find milestone \"{}\" in GitHub repo {}. Issue may not exist."
                      .format(release, repo_name))
        if show_found or strict:
            print("-" * 20)
            for task in self.tasks[plan_key]:
                gates = sorted(self.plans[plan_key][task][self.GATE].keys())
                for gate in gates:
                    if self.GHE in self.plans[plan_key][task][self.GATE][gate].keys():
                        print("Found task \"{}\" with gate \"{}\" in GitHub repo {} at url {}."
                              .format(task, gate, repo_name, self.plans[plan_key][task][self.GATE][gate][self.GHE]))
                        if strict:
                            print("\tThe following sections do not match between representations: {}"
                                  .format(", ".join(self.plans[plan_key][task][self.DIFF])))

    def validate_github_labels_against_plam(self, ghe_conn, repo_name, plan_key=None, show_found=False, strict=False):
        pass

    def validate_github_issues_against_plan(self, ghe_conn, repo_name, plan_key=None, show_found=False, strict=False):
        """ Retrieves all issues from a given GHE repo using the supplied ghe_connector, matches them against a plan
        in order to determine which plan tasks and gates do not have an equivalent GHE issue. Outputs results to
        console.

        :param ghe_conn: The gheconnector object to use for the issue retrieval.
        :param repo_name: The GHE repo from which to retrieve the issues for plan comparison.
        :param plan_key: The plan to match the issues against. Default to first plan passed to the planvalidator if not
            provided here.
        :param show_found: Option to show the issues which have been matched against the plan.
        :param strict: Option to validate issue details for assignee and release against plan.
        :return: None.
        """
        if plan_key is None:
            plan_key = self.primary_plan
        # get all of the issues
        issues = ghe_conn.get_issues(repo_name, events=False)
        # check each of the issues to see if it matches against
        for issue_number in issues.keys():
            self.find_issue_in_plan(issues[issue_number], plan_key=plan_key)
        for task in self.tasks[plan_key]:
            gates = sorted(self.plans[plan_key][task][self.GATE].keys())
            for gate in gates:
                if self.GHE not in self.plans[plan_key][task][self.GATE][gate].keys():
                    print("Unable to find task \"{}\" with gate \"{}\" in GitHub repo {}. Issue may not exist."
                          .format(task, gate, repo_name))
        if show_found or strict:
            print("-" * 20)
            for task in self.tasks[plan_key]:
                gates = sorted(self.plans[plan_key][task][self.GATE].keys())
                for gate in gates:
                    if self.GHE in self.plans[plan_key][task][self.GATE][gate].keys():
                        print("Found task \"{}\" with gate \"{}\" in GitHub repo {} at url {}."
                              .format(task, gate, repo_name, self.plans[plan_key][task][self.GATE][gate][self.GHE]))
                        if strict:
                            print("\tThe following sections do not match between representations: {}"
                                  .format(", ".join(self.plans[plan_key][task][self.DIFF])))

    def find_issue_in_plan(self, issue_json, plan_key=None):
        """ Takes issue json from GHE v3 API, infers a task and gate from it per the double label or title and label
        specs. If it can infer them, will attempt to match these against the plan_key, returns values it matches
        against. If it can match against a plan, returns these things.

        :param issue_json: JSON representation of the issue to match to plan
        :param plan_key: Which plan to check the issue against. Defaults to the first plan passed into the PlanValidator
            constructor if not otherwise specified.
        :return: Tuple consisting of Task id string, gate id string, and plan contribution EV.
        """
        if plan_key is None:
            plan_key = self.primary_plan
        if not plan_key:
            return None, None, 0

        title = issue_json.get("title")
        task = ""
        gate = ""
        contribution = 0.
        if title in self.plans[plan_key].keys():
            task = title
        elif ':' in title and title[:title.find(':')] in self.plans[plan_key].keys():
            task = title[:title.find(':')]
        else:
            for label_json in issue_json.get("labels"):
                label_name = str(label_json.get("name"))
                if label_name in self.plans[plan_key].keys():
                    task = label_name
                    break

        # if the we have found a task to associate the issue with, find a gate (if any) to associate with it. note that
        # if gates are defined and no gates are found on this issue, this issue will be worth 0
        if task:
            # if no gates, issue is worth full ev
            if "" in self.plans[plan_key][task][self.GATE].keys():
                pass
            # else find a gate label for the issue
            else:
                # have to loop through labels again since we can't guarantee ordering
                for label_json in issue_json.get("labels"):
                    # once we find a label that's a gate label, break (there can't be more than 1)
                    label_name = str(label_json.get("name"))
                    if label_name in self.plans[plan_key][task][self.GATE].keys():
                        gate = label_name
                        break

        if task and gate:
            self.plans[plan_key][task][self.GATE][gate][self.GHE] = issue_json.get("url")
            contribution = self.plans[plan_key][task][self.GATE][gate][self.EV]
            self.plans[plan_key][task][self.DIFF] = []
            for local_key, ghe_upper_key, ghe_lower_key in [(self.ENG, "assignee", "login"),
                                                            (self.REL, "milestone", "title")]:
                if issue_json[ghe_upper_key] is None:
                    ghe_value = ""
                else:
                    ghe_value = issue_json[ghe_upper_key][ghe_lower_key]
                if self.plans[plan_key][task][local_key] != ghe_value:
                    self.plans[plan_key][task][self.DIFF].append(local_key)

        return task, gate, contribution


def main(args, config_file=None, show_found=False, strict=False, repo=None):
    """ Builds a planvalidator.PlanValidator object with the options specified in the config_file, and then validates
    the first plan found in the config against the first repo found in the config (if not overridden here). Validation
    output is dumped to console.

    :param args: List of args to parse, generally obtained from the CLI.
    :param config_file: The config file to process. Can also be passed in via args.
    :param show_found: Whether to show the corresponding issues that have been found on GHE. Can also be passed in via
        args.
    :param strict: Whether to update issues found on GHE with data from the plan for assignee and milestone. Can also
        be passed in via args
    :param repo: Optional repo to search for issues. Otherwise defaults to the first repo specified in the config_file.
    :return: None
    """
    if "--config" in args:
        config_file = args[args.index("--config") + 1]
        config_opts = reportconfig.ReportConfiguration(config_file)
    else:
        raise Exception("Config file expected")

    if "--git-api-key" in args:
        config_opts.git_api_key = args[args.index("--git-api-key") + 1]

    if "--show_found" in args:
        show_found = True

    if "--strict" in args:
        strict = True

    ghe_conn = gheconnector.GitHubConnector(config_opts)
    if repo is None:
        repo = config_opts.git_repo_list[0]

    pv = PlanValidator(config_opts.metrics_ev_plan_files)
    pv.validate_github_issues_against_plan(ghe_conn, repo, show_found=show_found, strict=strict)


if __name__ == '__main__':
    main(sys.argv)
