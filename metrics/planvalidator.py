import sys
import gheconnector
import datetime
import csv
import reportconfig
import pprint


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

    # Human readable dict keys
    TASK_LIST = "Ordered Task List"
    GATE_LIST = "Ordered Gate List"
    GHE = "GitHub Issue"
    DIFF = "GitHub Diffs"

    def __init__(self, plan_file_list):
        self.primary_plan = ""
        self.plans = {}
        for index, plan_file in enumerate(plan_file_list):
            plan_key = self.generate_plan_file_key(plan_file)
            try:
                self.plans[plan_key] = self._generate_plan_data(plan_file)
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

        tasks = {}
        gates = {}
        releases = {}
        # task list is just to retain file ordering
        task_list = []

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
            if line[t] in tasks:
                raise Exception("Non-unique task name \"{}\" found in \"{}\". Aborting plan generation."
                                .format(line[t], plan_file))
            gates_line = line[g:]
            if not gates_line:
                gates_line = ["", '1.']
            if len(gates_line) % 2 != 0:
                gates_line.append('1.')

            tasks[line[t]] = self._process_plan_task(line[t], line[n], line[r], float(line[v]), start_date, end_date,
                                                     gates_line)
            task_list.append(line[t])

            for gate in tasks[line[t]][self.GATE_LIST]:
                if gate:
                    gates[gate] = {self.GATE: gate, self.GHE: None}
            releases[line[r]] = {self.REL: line[r], self.GHE: None}
        plan = {self.TASK: tasks,
                self.TASK_LIST: task_list,
                self.GATE: gates,
                self.REL: releases}
        return plan

    def _process_plan_task(self, task_name, engineer, release, task_ev, start_date, end_date, gates):
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
        task_dict = {self.TASK: task_name,
                     self.REL: release,
                     self.ENG: engineer,
                     self.START: start_date,
                     self.END: end_date,
                     self.EV: task_ev,
                     self.GATE_LIST: [],
                     self.GHE: None}
        for index, gate in enumerate(gates):
            if index % 2 == 1:
                continue
            if not gate and index > 0:
                continue
            elif not gate and index == 0:
                gates = [""]
            task_dict[self.GATE_LIST].append(gate)

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

            task_dict[gate] = {self.TASK: task_name,
                               self.GATE: gate,
                               self.START: task_start,
                               self.END: task_end,
                               self.REL: release,
                               self.ENG: engineer,
                               self.EV: contribution,
                               self.GHE: None,
                               self.DIFF: None
                               }
            # NOTE - if the task has a self.GHE entry, it's a link to a label because task_labels was turned on
        return task_dict

    def find_issue_in_plan(self, issue, plan_key=None):
        """ Takes issue json from GHE v3 API, infers a task and gate from it per the double label or title and label
        specs. If it can infer them, will attempt to match these against the plan_key, returns values it matches
        against. If it can match against a plan, returns these things.

        :param issue: JSON representation of the issue to match to plan
        :param plan_key: Which plan to check the issue against. Defaults to the first plan passed into the PlanValidator
            constructor if not otherwise specified.
        :return: Tuple consisting of Task id string, gate id string, and plan contribution EV.
        """
        plan_key = plan_key or self.primary_plan
        if not plan_key:
            return None, None, 0
        tasks = self.plans[plan_key][self.TASK]

        task_name = None
        gate_name = None
        contribution = 0.

        title = issue.get("title")
        if title in list(tasks.keys()):
            task_name = title
        elif ':' in title and title[:title.find(':')] in list(tasks.keys()):
            task_name = title[:title.find(':')]
        else:
            for label_json in issue.get("labels"):
                label_name = str(label_json.get("name"))
                if label_name in list(tasks.keys()):
                    task_name = label_name
                    break

        # if the we have found a task to associate the issue with, find a gate (if any) to associate with it. note that
        # if gates are defined and no gates are found on this issue, this issue will be worth 0
        if task_name:
            # if no gates, issue is worth full ev
            if "" in tasks[task_name][self.GATE_LIST]:
                gate_name = ""
            # else find a gate label for the issue
            else:
                # have to loop through labels again since we can't guarantee ordering
                for label_json in issue.get("labels"):
                    # once we find a label that's a gate label, break (there can't be more than 1)
                    label_name = str(label_json.get("name"))
                    if not gate_name and label_name in list(tasks[task_name].keys()):
                        gate_name = label_name
                    # labels.append(label_name)
            if gate_name or gate_name == "":
                tasks[task_name][gate_name][self.DIFF] = []
                tasks[task_name][gate_name][self.GHE] = issue
                contribution = tasks[task_name][gate_name][self.EV]
                for local_key, ghe_upper_key, ghe_lower_key in [(self.ENG, "assignee", "login"),
                                                                (self.REL, "milestone", "title")]:
                    if ghe_upper_key in list(issue.keys()) and issue[ghe_upper_key]:
                        ghe_value = issue[ghe_upper_key][ghe_lower_key]
                    else:
                        ghe_value = ""
                    if tasks[task_name][gate_name][local_key] != ghe_value:
                        diff_string = "{} - Plan: {}, GitHub: {}" \
                            .format(local_key, tasks[task_name][gate_name][local_key], ghe_value)
                        tasks[task_name][gate_name][self.DIFF].append(diff_string)
        return task_name, gate_name, contribution

    def validate_gates_against_github(self, ghe_conn, repo_name, plan_key=None, show_found=False,
                                      update=False, create=False, task_labels=False, gates=None):
        """ Retrieves all labels from a given GitHub repo using the supplied ghe_connector, matches them against a
        plan in order to determine which plan gates (and potentially tasks)  do not have an equivalent GitHub label.
        Outputs results to console.

        :param ghe_conn: The gheconnector object to use for the issue retrieval.
        :param repo_name: The GHE repo from which to retrieve the issues for plan comparison.
        :param plan_key: The plan to match the issues against. Defaults to first plan passed to the planvalidator if not
            provided here.
        :param show_found: Option to show the items that have been found on GitHub. Will differentiate between found
            matching and non-matching items.
        :param update: Unused. Retained for future work.
        :param create: Option to create missing items when detected. Defaults to False.
        :param task_labels:
        :param gates: Overrides the default gate search set. Intended only to be used with task_labels option.
        :return:
        """
        plan_key = plan_key or self.primary_plan
        if task_labels:
            self.validate_gates_against_github(ghe_conn, repo_name, plan_key, show_found, update, create,
                                               task_labels=False, gates=self.plans[plan_key][self.TASK])
        labels = ghe_conn.get_labels(repo_name)
        gates = gates or self.plans[plan_key][self.GATE]
        print("\n{} Missing Labels {} ".format("-" * 10, "-" * 10))
        for gate in list(gates.keys()):
            if gate not in list(labels.keys()):
                print("Unable to find label \"{}\" in GitHub repo {}.".format(gate, repo_name))
                if create:
                    print("** Creating \"{}\"".format(gate))
                    ghe_conn.create_label(repo_name, gate)
        if update or show_found:
            pass
            # Nothing to do in this case, retained for consistency
        if show_found:
            print("\n{} Found Labels {} ".format("-" * 10, "-" * 10))
            for gate in list(gates.keys()):
                if gate in list(labels.keys()):
                    print("Found label \"{}\" in GitHub repo {} at url {}."
                          .format(gate, repo_name, labels[gate]["url"]))

    def validate_releases_against_github(self, ghe_conn, repo_name, plan_key=None, show_found=False,
                                         update=False, create=False, task_labels=False):
        """ Retrieves all milestones from a given GitHub repo using the supplied ghe_connector, matches them against a
        plan in order to determine which plan releases do not have an equivalent GitHub milestone. Outputs results to
        console.

        :param ghe_conn: The gheconnector object to use for the issue retrieval.
        :param repo_name: The GHE repo from which to retrieve the issues for plan comparison.
        :param plan_key: The plan to match the issues against. Defaults to first plan passed to the planvalidator if not
            provided here.
        :param show_found: Option to show the items that have been found on GitHub. Will differentiate between found
            matching and non-matching items.
        :param update: Unused. Retained for future work.
        :param create: Option to create missing items when detected. Defaults to False.
        :param task_labels: Unused. Retained for consistency.
        :return:
        """
        plan_key = plan_key or self.primary_plan
        milestones = ghe_conn.get_milestones(repo_name)
        for milestone in list(milestones.values()):
            name = milestone["title"]
            milestones[name] = milestone
        releases = self.plans[plan_key][self.REL]
        print("\n{} Missing Releases {} ".format("-" * 10, "-" * 10))
        for release in list(releases.keys()):
            if release not in list(milestones.keys()):
                print("Unable to find milestone \"{}\" in GitHub repo {}.".format(release, repo_name))
                if create:
                    print("** Creating \"{}\"".format(release))
                    milestone = ghe_conn.create_milestone(repo_name, release)
                    # since we're aliasing the milestones for easy lookup, alias this new one too
                    ghe_conn.milestones[repo_name][release] = milestone
        if update or show_found:
            pass
            # Nothing to do in this case, retained in case due_date calculation and updating was wanted.
        if show_found:
            print("\n{} Found Releases {} ".format("-" * 10, "-" * 10))
            for release in list(releases.keys()):
                if release in list(milestones.keys()):
                    print("Found milestone \"{}\" in GitHub repo {} at url {}."
                          .format(release, repo_name, milestones[release]["url"]))

    def validate_plan_against_github(self, ghe_conn, repo_name, plan_key=None, show_found=False,
                                     update=False, create=False, task_labels=False):
        """ Retrieves all issues from a given GitHub repo using the supplied ghe_connector, matches them against a plan
        task/gate issue in order to determine which plan task/gate issues do not have an equivalent GitHub issue.
        Outputs results to console.

        :param ghe_conn: The gheconnector object to use for the issue retrieval.
        :param repo_name: The GHE repo from which to retrieve the issues for plan comparison.
        :param plan_key: The plan to match the issues against. Defaults to first plan passed to the planvalidator if not
            provided here.
        :param show_found: Option to show the items that have been found on GitHub. Will differentiate between found
            matching and non-matching items.
        :param update: Option to update found but non-matching items when detected. Defaults to False.
        :param create: Option to create missing items when detected. Defaults to False.
        :param task_labels: Option to label a task with it's task as well as gate when creating new tasks. Will not
            update existing tasks to use task label scheme.
        :return: None.
        """
        plan_key = plan_key or self.primary_plan
        tasks = self.plans[plan_key][self.TASK]
        task_list = self.plans[plan_key][self.TASK_LIST]
        # get all of the issues
        issues = ghe_conn.get_issues(repo_name, events=False)
        # check each of the issues to see if it matches against
        for issue_number in list(issues.keys()):
            self.find_issue_in_plan(issues[issue_number], plan_key=plan_key)
        print("\n{} Missing Plan Items {} ".format("-" * 10, "-" * 10))
        for task in task_list:
            for gate in tasks[task][self.GATE_LIST]:
                if not tasks[task][gate][self.GHE]:
                    print("Unable to find task \"{}\" with gate \"{}\" in GitHub repo {}. Issue may not exist."
                          .format(task, gate, repo_name))
                    if create:
                        self.create_plan_task(ghe_conn, repo_name, tasks[task][gate], task_labels)
        if update or show_found:
            print("\n{} Non-matching Plan Items {} ".format("-" * 10, "-" * 10))
            for task in task_list:
                for gate in tasks[task][self.GATE_LIST]:
                    if tasks[task][gate][self.GHE] and tasks[task][gate][self.DIFF]:
                        print("Found task \"{}\" with gate \"{}\" in GitHub repo {} at url {}. "
                              "Some fields did not match: \n\t{}"
                              .format(task, gate, repo_name, tasks[task][gate][self.GHE]["url"],
                                      "\n\t".join(tasks[task][gate][self.DIFF])))
                        if update:
                            self.update_plan_task(ghe_conn, repo_name, tasks[task][gate])
        if show_found:
            print("\n{} Matched Plan Items {} ".format("-" * 10, "-" * 10))
            for task in task_list:
                for gate in tasks[task][self.GATE_LIST]:
                    if tasks[task][gate][self.GHE] and not tasks[task][gate][self.DIFF]:
                        print("Found task \"{}\" with gate \"{}\" in GitHub repo {} at url {}."
                              .format(task, gate, repo_name, tasks[task][gate][self.GHE]["url"]))

    def create_plan_task(self, ghe_conn, repo_name, task_object, task_labels=False):
        """ Creates a GitHub issue from an internal plan task/gate issue representation.

        :param ghe_conn: The GitHubConnection object responsible for linking with GitHub.
        :param repo_name: The name of the repo to make these changes in.
        :param task_object: Plan representation of the task/gate issue that has already been matched to its GitHub
            equivalent (generally with the find_issue_in_plan() method).
        :param task_labels: Whether to label the issue with the task name as well.
        :return:
        """
        print("** Creating \"{}: {}\"".format(task_object[self.TASK], task_object[self.GATE]))
        task = task_object[self.TASK]
        gate = task_object[self.GATE]
        milestone = ghe_conn.milestones[repo_name][task_object[self.REL]]["number"]
        assignees = [task_object[self.ENG]]
        labels = [gate]
        if task_labels:
            labels.append(task)
        name = "{}: {}".format(task, gate)
        try:
            ghe_conn.create_issue(repo_name, issue_name=name, labels_list=labels, milestone_number=milestone,
                                  assignees_list=assignees)
        except:
            # assignee is one we can retry
            print("WARN: Error while creating issue. Attempting to create without assignee.")
            ghe_conn.create_issue(repo_name, issue_name=name, labels_list=labels, milestone_number=milestone)

    def update_plan_task(self, ghe_conn, repo_name, task_object):
        """ Updates the GitHub representation of a task/gate issue and pushes the update to GitHub. Will attempt to
        update the milestone and assignee separately, so that a failure of one does not prevent an incremental change
        to the issue.

        :param ghe_conn: The GitHubConnection object responsible for linking with GitHub.
        :param repo_name: The name of the repo to make these changes in.
        :param task_object: Plan representation of the task/gate issue that has already been matched to its GitHub
            equivalent (generally with the find_issue_in_plan() method).
        :return:
        """
        issue = task_object[self.GHE]
        print("** Updating \"{}: {}\"".format(task_object[self.TASK], task_object[self.GATE]))
        # check and update release
        milestone = ghe_conn.milestones[repo_name][task_object[self.REL]]
        try:
            issue["milestone"] = ghe_conn.milestones[repo_name][task_object[self.REL]]
            ghe_conn.update_issue(issue)
        except:
            print("ERROR: Unable to update issue milestone.")

        # check and update assignee
        try:
            issue["assignees"] = [{"login": task_object[self.ENG]}]
            ghe_conn.update_issue(issue)
        except:
            print("ERROR: Unable to update issue assignee.")


def main(args, config_file=None, config_opts=None, ghe_conn=None, repo=None, show_found=False,
         update=False, create=False, task_labels=False, dump=False):
    """ Builds a planvalidator.PlanValidator object with the options specified in the config_file, and then validates
    the first plan found in the config against the first repo found in the config (if not overridden here). Validation
    results are output to console.

    :param args: List of args to parse, generally obtained from the CLI.
    :param config_file: The config file to process. Can also be passed in via "--config <file>'' arg.
    :param config_opts: Optional pre-created reportconfig.ReportConfiguration object. Will overwrite config_file created
        object if passed.
    :param ghe_conn: Optional pre-created gheconnector.GitHubConnecgtor object. Will prevent the script from generating
        one with config_opts if specified.
    :param repo: Optional repo to search for issues. Otherwise defaults to the first repo specified in config_opts.
    :param show_found: Whether to show the corresponding issues that have been found on GHE, both matching and non-
        matching. Can also be passed in via "--show-found" arg.
    :param update: Whether to update issues found on GitHub with data from the plan for assignee and milestone. Can also
        be passed in via "--update" arg.
    :param create: Whether to create missing issues on GitHub with date from the plan. Can also be passed in via the
        "--create" arg.
    :param task_labels: Whether to create a label for the task name as well as its gate. Can also be passed in via the
        "--task-labels" arg.
    :param dump: If True, will redirect output to file and dump plan and issue dicts to local files for debug purposes.
        Can also be passed in via the "--dump" arg.
    :return: None
    """
    show_found = show_found or "--show-found" in args
    update = update or "--update" in args
    create = create or "--create" in args
    task_labels = task_labels or "--task-labels" in args
    dump = dump or "--dump" in args

    if dump:
        sys.stdout = open("output.txt", "w")

    if "--config" in args:
        config_file = args[args.index("--config") + 1]

    config_opts = config_opts or reportconfig.ReportConfiguration(config_file)
    if "--git-api-key" in args:
        config_opts.git_api_key = args[args.index("--git-api-key") + 1]

    ghe_conn = ghe_conn or gheconnector.GitHubConnector(config_options=config_opts)
    if repo is None:
        repo = config_opts.git_repo_list[0]

    pv = PlanValidator(config_opts.metrics_ev_plan_files)
    pv.validate_gates_against_github(ghe_conn, repo, show_found=show_found, update=update, create=create,
                                     task_labels=task_labels)
    pv.validate_releases_against_github(ghe_conn, repo, show_found=show_found, update=update, create=create,
                                        task_labels=task_labels)
    pv.validate_plan_against_github(ghe_conn, repo, show_found=show_found, update=update, create=create,
                                    task_labels=task_labels)
    if dump:
        with open("plan.txt", 'w') as fp:
            pprint.pprint(pv.plans, stream=fp)
        with open("issues.txt", 'w') as fp:
            pprint.pprint(ghe_conn.issues, stream=fp)
        sys.stdout.close()


if __name__ == '__main__':
    main(sys.argv)
