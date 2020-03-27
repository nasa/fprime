import requests
import random
import time


class GitHubConnector:
    """
    Helper class for performing various GitHub and ZenHub operations.

    This class caches some GitHub and ZenHub objects locally, so only the first call will use rate limited resources in
    most circumstances. To force a re-acquisition of the GHE object, empty the appropriate self.<type>[owner/repo]
    dictionary.
    """

    API_ENDPOINT = "/api/v3/repos"
    RAW_ENDPOINT = "/raw"

    BOARD_SUFFIX = "/board"
    EVENTS_SUFFIX = "/events"
    ISSUES_SUFFIX = "/issues"
    CONTENTS_SUFFIX = "/contents"
    LABELS_SUFFIX = "/labels"
    MILESTONES_SUFFIX = "/milestones"

    def __init__(self, github_url=None, github_key=None, github_issue_params=None, zenhub_url=None, zenhub_key=None,
                 config_options=None, github_limit_wait=False, zenhub_limit_wait=False, auto_retry_wait=10000):
        """
        Initializer for the gheconnector.GitHubConnector object.
        :param github_url:
        :param github_key:
        :param github_issue_params:
        :param zenhub_url:
        :param zenhub_key:
        :param config_options: A reportconfig.ReportConfig object that contains a number of parameters to identify the
            GitHub and ZenHub servers.
        :param github_limit_wait: If true, request will wait for rate to reset when rate limit reached, and will then
            re-attempt the request. If false, request will raise an IOError when rate limit is reached. Defaults to
            "False", because the GitHub reset time can be long.
        :param zenhub_limit_wait: If true, request will wait for rate to reset when rate limit reached, and will then
            re-attempt the request. If false, request will raise an IOError when rate limit is reached. Defaults to
            "True", because the ZenHub reset time is short and we can generally afford it.
        :param auto_retry_wait: A number of ms to wait for the rate limit to reset, regardless of other settings. Don't
            want to abort when it'll reset shortly. Defaults to 10000ms.
        """
        self._github_limit_wait = github_limit_wait
        self._zenhub_limit_wait = zenhub_limit_wait
        self._auto_retry_wait = auto_retry_wait

        if config_options:
            self.github_url = config_options.git_base_url
            self.github_key = config_options.git_api_key
            self.github_issue_params = {}
            for param in config_options.git_params_list:
                entry = param.split(":")
                self.github_issue_params[entry[0]] = entry[1]
            self.zenhub_url = config_options.zen_base_url
            self.zenhub_key = config_options.zen_api_key
        else:
            self.github_url = github_url
            self.github_key = github_key
            self.github_issue_params = github_issue_params or {}
            self.zenhub_url = zenhub_url
            self.zenhub_key = zenhub_key

        if self.github_url is None or self.github_url == '' or self.github_key is None or self.github_key == '':
            self.github = False
        else:
            self.found_submodules = {}
            self.github = True
            self.github_auth_header = {"Authorization": "token " + self.github_key}
            self.github_limit = 1
            self.github_remaining = 1
            self.github_reset = 0
            self.repos = {}
            self.issues = {}
            self.labels = {}
            self.milestones = {}

        if self.zenhub_url is None or self.zenhub_url == '' or self.zenhub_key is None or self.zenhub_key == '':
            self.zenhub = False
        else:
            self.zenhub = True
            self.zenhub_auth_header = {"X-Authentication-Token": self.zenhub_key}
            self.zenhub_limit = 1
            self.zenhub_api_used = 0
            self.zenhub_reset = 0

    def _scrub_path(self, path):
        if path.startswith('/'):
            path = path[1:]
        if path.endswith('/'):
            path = path[:-1]
        return path

    def _resolve_submodule_path(self, repo_name, path, ref, complete=True):
        path_steps = path.split('/')
        current_path = ""
        for index, step in enumerate(path_steps):
            current_path += '/' + step
            if not complete and index == len(path_steps) - 1:
                break
            if (repo_name, ref, current_path) in list(self.found_submodules.keys()):
                repo_name, ref, prefix_path = self.found_submodules[(repo_name, ref, current_path)]
                current_path = ""
        return repo_name, current_path, ref

    def _find_submodules_in_path(self, repo_name, path, ref):
        path_steps = path.split('/')
        current_path = ""
        for step in path_steps:
            current_path += "/" + step
            github_contents_url = self.github_url + self.API_ENDPOINT + '/' + repo_name + self.CONTENTS_SUFFIX + current_path
            params = {"ref": ref}
            content, h, s = self._github_request(github_contents_url, params=params, follow_next=False)
            try:
                submodule_repo = content['submodule_git_url'][len(self.github_url) + 1:-4]
                submodule_ref = content['sha']
                self.found_submodules[(repo_name, ref, current_path)] = (submodule_repo, submodule_ref, current_path)
                repo_name = submodule_repo
                ref = submodule_ref
                current_path = ""
            except TypeError:
                # it's either a file, directory, or something else we don't need to process for.
                continue
        return

    def _get_contents_object(self, repo_name, path, ref, traverse_submodules, complete):
        if not self.github:
            return None
        if traverse_submodules:
            repo_name, path, ref = self._resolve_submodule_path(repo_name, path, ref, complete=complete)
        github_contents_url = self.github_url + self.API_ENDPOINT + '/' + repo_name + self.CONTENTS_SUFFIX + '/' + path + '/'
        params = {"ref": ref}
        object_json, h, s = self._github_request(github_contents_url, params=params, follow_next=False)
        return object_json

    def get_repo(self, repo):
        """

        :param repo:
        :return:
        """
        if repo not in list(self.repos.keys()):
            github_repo_url = self.github_url + self.API_ENDPOINT + '/' + repo
            github_repo_data, github_repo_headers, status = self._github_request(github_repo_url)
            self.repos[repo] = github_repo_data
        return self.repos[repo]

    def get_contents(self, repo_name, path="", ref="master", traverse_submodules=False):
        """
        Retrieves the directory contents from a GHE repo. Will display submodule contents if traverse is true, and will
        display non-nested submodule entries if traverse is false. Should not be used to retrieve submodule entries, as
        get_submodule_object is intended for that use.
        :param repo_name: GHE repository name, including owner. ex: "organization/repository"
        :param ref: Branch / tag / ref of the repository to look in. Defaults to master.
        :param path: Relative path to the directory from the root of the repository. Expects Linux/URL like file
            paths. Defaults to top level of repository.
        :param traverse_submodules: Attempts to validate directory structure and traverse any discovered submodule
            links. Expensive, since it has to check each directory step, but found results will be cached to save
            time on later attempts. Will cache submodule repo name, submodule ref, and path in parent repo to enable
            multiple repo matches.
        :return: Directory contents as json
        """
        path = self._scrub_path(path)
        try:
            # this will 404 if it traverses a submodule. it will not display contents if it ends at a submodule.
            content = self._get_contents_object(repo_name, path, ref, traverse_submodules, complete=True)
        except requests.HTTPError as err:
            if err.response.status_code == 404 and traverse_submodules:
                # find new submodules, if any
                self._find_submodules_in_path(repo_name, path, ref)
                # check to see if resolving the path with potential new submodules is different, implying something
                # interesting. if not, something is weird so raise err
                if (repo_name, path, ref) != self._resolve_submodule_path(repo_name, path, ref):
                    return self.get_contents(repo_name, path, ref, traverse_submodules)
                else:
                    raise err
            else:
                raise err
        # before passing back content, see if it's a submodule. if it is, add it and retry to get actual contents
        if content and traverse_submodules:
            try:
                submodule_repo = content['submodule_git_url'][len(self.github_url) + 1:-4]
                submodule_ref = content['sha']
                # should be safe to assume that the current path has been resolved via the previous traversals
                self.found_submodules[(repo_name, ref, path)] = (submodule_repo, submodule_ref, path)
                return self.get_contents(repo_name, path, ref, traverse_submodules)
            except TypeError:
                # it's not a submodule, so return the regular content response
                pass
        return content

    def get_contents_object(self, repo_name, path="", ref="master", traverse_submodules=False):
        """
        Retrieves a submodule org/repo and sha ref from a submodule object in a repo. Verifies that it is grabbing a
            submodule object, so it can be used to differentiate if needed.
        :param repo_name: field to specify a repo directly to retrieve the submodule object from
        :param ref: field to specify a ref directly to retrieve the submodule object from
        :param path: field to specify a path directly for the location of the submodule object
        :param traverse_submodules: Attempts to validate directory structure and traverse any discovered submodule
            links. Expensive, since it has to check each directory step, but found results will be cached to save
            time on later attempts. Will cache submodule repo name, submodule ref, and path in parent repo to enable
            multiple repo matches.
        :return: dictionary of object contents response
        """
        path = self._scrub_path(path)
        content = self._get_contents_object(repo_name, path, ref, traverse_submodules, complete=False)
        try:
            file_type = content['type']
        except TypeError:
            # directory, not a file or submodule.
            pass
        return content

    def get_raw_file(self, repo_name, path, ref="master", traverse_submodules=False):
        """
        Light wrapper around the requests library to retrieve a raw file from GHE. Doesn't call the self._github_request
            method because it doesn't receive a json response, and it doesn't need to because it doesn't count against
            api calls.
        :param repo_name: GHE repository name, including owner. ex: "organization/repository"
        :param ref: Branch of the repository to look in.
        :param path: Relative path to the file from the root of the repository. Requires Linux/URL like file paths.
        :param traverse_submodules:
        :return: response content (raw file)
        """
        if not self.github:
            return None
        path = self._scrub_path(path)
        content = self._get_contents_object(repo_name, path, ref, traverse_submodules, complete=True)
        github_file_url = content["download_url"]
        body, h, s = self._github_request(github_file_url, raw=True)
        return body

    def get_issues(self, repo_name, limit=-1, events=True):
        """
        Gathers multiple issues in a repository, from most current down to a specified limit. Optionally includes issue
        events.

        NOTE: This function does not wrap the get_single_issue function, as GHE includes the ability to bulk get issues
        with a request. This saves some time with the request calls, at the expense of some slightly duplicated code.

        :param repo_name: GHE repository name, including owner. ex: "organization/repository"
        :param limit: Optional limit on the issues returned. Issues are retrieved from most current (largest number) and
            then down until the limit is reached. A limit of <=0 will cause all issues in the repository to be gathered.
        :param events: Optional boolean to indicated whether events are gathered for each issue. Events must be gathered
            for each issue, which adds a large number of requests to the process and is time consuming. Defaults to True
            but should be turned off if not needed.
        :return: A dictionary of all issues in the repository.
        """
        if not self.github:
            return {}
        if self.issues.get(repo_name) is None:
            self.issues[repo_name] = {}
        github_repo_id = str(self.get_repo(repo_name).get("id"))
        github_repo_url = self.github_url + self.API_ENDPOINT + '/' + repo_name
        github_issues_url = github_repo_url + self.ISSUES_SUFFIX
        # get all of the issues, check for exceeding the limit later
        github_issues, headers, status = self._github_request(github_issues_url, params=self.github_issue_params)
        # iterate through all of the issues retrieved
        for issue_json in github_issues:
            issue_number = issue_json.get("number")
            # check if we should stop at a particular number, if not store in dict
            if limit >= issue_number:
                break
            # do zenhub for the issue, if applicable
            if self.zenhub:
                zenhub_issue_url = self.zenhub_url + '/' + github_repo_id + self.ISSUES_SUFFIX
                zenhub_issue_data, zenhub_issue_headers, status = \
                    self._zenhub_request(zenhub_issue_url + "/" + str(issue_number))
                issue_json.update(zenhub_issue_data.json())
            else:
                zenhub_issue_url = ""
            # do events for the issue, if applicable
            if events:
                issue_json["events"] = self._get_issue_events(issue_number, github_issues_url, zenhub_issue_url)
            # store completed issue
            self.issues[repo_name][issue_number] = issue_json
        return self.issues[repo_name]

    def get_single_issue(self, repo_name, issue_number, events=True):
        """
        Gathers multiple issues in a repository, from most current down to a specified limit. Optionally includes issue
        events.
        :param repo_name: GHE repository name, including owner. ex: "organization/repository"
        :param issue_number: The specific issue number to be retrieved.
        :param events: Optional boolean to indicated whether events are gathered for each issue. Events must be gathered
            for each issue, which adds a large number of requests to the process and is time consuming. Defaults to True
            but should be turned off if not needed.
        :return: A dictionary of all issues in the repository.
        """

        if self.issues.get(repo_name) is None:
            self.issues[repo_name] = {}
        if not self.github:
            return {}
        if self.issues.get(repo_name).get(issue_number) is None:
            github_repo_url = self.github_url + self.API_ENDPOINT + '/' + repo_name
            github_issues_url = github_repo_url + self.ISSUES_SUFFIX
            issue_json, headers, status = \
                self._github_request(github_issues_url + "/" + str(issue_number))
            # do zenhub for the issue, if applicable
            if self.zenhub:
                github_repo_id = self.get_repo(repo_name).get("id")
                zenhub_issues_url = self.zenhub_url + '/' + github_repo_id + self.ISSUES_SUFFIX
                zenhub_issue_data, zenhub_issue_headers, status = \
                    self._zenhub_request(zenhub_issues_url + "/" + str(issue_number))
                issue_json.update(zenhub_issue_data.json())
            else:
                zenhub_issues_url = ""
            # do events for the issue, if applicable
            if events:
                issue_json["events"] = self._get_issue_events(issue_number, github_issues_url, zenhub_issues_url)
            # store completed issue
            self.issues[repo_name][issue_number] = issue_json
        return self.issues[repo_name][issue_number]

    def _get_issue_events(self, issue_number, github_issues_url, zenhub_issues_url=""):
        """
        A function to retrieve the event history of an issue.

        NOTE: GHE and ZHE issue urls here are slightly different in scope,
        :param issue_number: The specific issue number to retrieve the events for.
        :param github_issues_url: The general issues endpoint for GHE.
        :param zenhub_issues_url: The endpoint for this specific issue for ZHE
        :return: A list of events for the issue (intended to be stored in issue_json["events"]).
        """
        events = []
        github_issue_events_url = github_issues_url + "/" + str(issue_number) + self.EVENTS_SUFFIX
        github_issue_events_data, h, s = self._github_request(github_issue_events_url)
        events += github_issue_events_data
        if self.zenhub and zenhub_issues_url:
            zenhub_issue_events_url = zenhub_issues_url + "/" + str(issue_number) + self.EVENTS_SUFFIX
            zenhub_issue_events_data, h, s = self._zenhub_request(zenhub_issue_events_url)
            events += zenhub_issue_events_data
        return events

    def get_labels(self, repo_name):
        if not self.github:
            return {}
        if repo_name not in self.labels:
            self.labels[repo_name] = {}
        # /repos/:owner/:repo/labels
        github_label_url = self.github_url + self.API_ENDPOINT + '/' + repo_name + self.LABELS_SUFFIX
        github_label_data, h, s = self._github_request(github_label_url)
        for label_json in github_label_data:
            label_name = label_json["name"]
            self.labels[repo_name][label_name] = label_json
        return self.labels[repo_name]

    def get_single_label(self, repo_name, label_name):
        if not self.github:
            return {}
        if repo_name not in self.labels:
            self.labels[repo_name] = {}
        if label_name not in list(self.labels.keys()):
            # /repos/:owner/:repo/labels
            github_label_url = self.github_url + self.API_ENDPOINT + '/' + repo_name + self.LABELS_SUFFIX \
                               + "/" + label_name
            github_label_data, h, s = self._github_request(github_label_url)
            self.labels[repo_name][label_name] = github_label_data
        return self.labels[repo_name][label_name]

    def get_milestones(self, repo_name, params=None):
        if not self.github:
            return {}
        if repo_name not in self.milestones:
            self.milestones[repo_name] = {}
        # /repos/:owner/:repo/labels
        params = params or self.github_issue_params
        github_milestone_url = self.github_url + self.API_ENDPOINT + '/' + repo_name + self.MILESTONES_SUFFIX
        github_milestone_data, h, s = self._github_request(github_milestone_url, params=params)
        for milestone_json in github_milestone_data:
            milestone_number = milestone_json["number"]
            self.milestones[repo_name][milestone_number] = milestone_json
        return self.milestones[repo_name]

    def get_single_milestone(self, repo_name, milestone_number):
        if not self.github:
            return {}
        if repo_name not in self.milestones:
            self.milestones[repo_name] = {}
        if milestone_number not in list(self.milestones.keys()):
            # /repos/:owner/:repo/labels
            github_milestone_url = self.github_url + self.API_ENDPOINT + '/' + repo_name + self.MILESTONES_SUFFIX \
                                   + "/" + milestone_number
            github_milestone_data, h, s = self._github_request(github_milestone_url)
            milestone_number = github_milestone_data["number"]
            self.milestones[repo_name][milestone_number] = github_milestone_data
        return self.labels[repo_name][milestone_number]

    def create_issue(self, repo_name, issue_name, body=None, assignees_list=None, milestone_number=None, labels_list=None):
        """

        :param repo_name:
        :param issue_name:
        :param body:
        :param assignees_list:
        :param milestone_number:
        :param labels_list:
        :return:
        """
        issue_json = {"title": issue_name}
        if body:
            issue_json["body"] = body
        if assignees_list:
            issue_json["assignees"] = assignees_list
        if milestone_number:
            issue_json["milestone"] = milestone_number
        if labels_list:
            issue_json["labels"] = labels_list
        github_repo_url = self.github_url + self.API_ENDPOINT + '/' + repo_name
        github_issues_url = github_repo_url + self.ISSUES_SUFFIX
        github_issue_data, h, s = self._github_post(github_issues_url, issue_json)
        issue_number = github_issue_data["number"]
        if repo_name not in list(self.issues.keys()):
            self.issues[repo_name] = {}
        self.issues[repo_name][issue_number] = github_issue_data
        return self.issues[repo_name][issue_number]

    def create_label(self, repo_name, label_name, label_color=None, description=""):
        """

        :param repo_name: Owner/Repo in which to create the label.
        :param label_name: Name of new label.
        :param label_color: 6 digit hex color code for label color, without preceding '#'. Will generate randomly if
            not provided.
        :param description: Optional label description.
        :return: Dict of created label json.
        """
        label_color = label_color or "%06x" % random.randint(0, 0xFFFFFF)
        label_json = {"name": label_name, "color": label_color, "description": description}
        github_repo_url = self.github_url + self.API_ENDPOINT + '/' + repo_name
        github_labels_url = github_repo_url + self.LABELS_SUFFIX
        github_issue_data, h, s = self._github_post(github_labels_url, label_json)
        if repo_name not in list(self.labels.keys()):
            self.labels[repo_name] = {}
        self.labels[repo_name][label_name] = github_issue_data
        return self.labels[repo_name][label_name]

    def create_milestone(self, repo_name, milestone_name, state="open", description="", due_date=None):
        """

        :param repo_name: Owner/Repo in which to create the milestone.
        :param milestone_name: Name of the milestone.
        :param state: Optional state of the milestone, either "open" or "closed". Defaults to "open".
        :param description: Optional description of the milestone.
        :param due_date: Optional due date of the milestone.
        :return: Dict of created milestone json.
        """
        milestone_json = {"title": milestone_name, "state": state, "description": description}
        if due_date:
            if len(due_date) == 10:
                due_date += "T08:00:00Z"
            milestone_json["due_on"] = due_date
        github_repo_url = self.github_url + self.API_ENDPOINT + '/' + repo_name
        github_milestones_url = github_repo_url + self.MILESTONES_SUFFIX
        github_issue_data, h, s = self._github_post(github_milestones_url, milestone_json)
        milestone_number = github_issue_data["number"]
        if repo_name not in list(self.milestones.keys()):
            self.milestones[repo_name] = {}
        self.milestones[repo_name][milestone_number] = github_issue_data
        return self.milestones[repo_name][milestone_number]

    def update_issue(self, issue):
        """ Updates GitHub to match the internal representation of the issue.

        To add or change a milestone, set issue["milestone"] = <new_milestone>.
        To remove a milestone, set issue["milestone"] = None
        To add a new assignee, call issue["assignees"].append({"login": <assignee_login>})
        To remove an assignee, delete their entry from issue["assignees"]
        To add a new label, call issue["labels"].append({"name": <label_name>})
        To remove a label, delete its entry from issue["labels"]

        After changes have been made, call this function and pass in the updated issue dict.

        :param issue: The local issue that you with to push to GitHub.
        :return: Dict of updated issue json.
        """
        issue_json = {"title": issue["title"],
                      "body": issue["body"],
                      "assignees": [user["login"] for user in issue["assignees"]],
                      "milestone": issue["milestone"]["number"] if issue["milestone"] else None,
                      "labels": [label["name"] for label in issue["labels"]],
                      "state": issue["state"]}
        issue, h, s = self._github_post(issue["url"], issue_json)
        return issue

    def update_label(self, label):
        """

        :param label:
        :return: Dict of updated label json.
        """
        label_json = {"name": label["name"],
                      "color": label["color"],
                      "description": label["description"] if "description" in list(label.keys()) else ""}
        label, h, s = self._github_post(label["url"], label_json)
        return label

    def update_milestone(self, milestone):
        """

        :param milestone:
        :return: Dict of updated milestone json.
        """
        milestone_json = {"title": milestone["title"],
                          "due_on": milestone["due_on"],
                          "description": milestone["description"] if "description" in list(milestone.keys()) else "",
                          "state": milestone["state"]}
        milestone, h, s = self._github_post(milestone["url"], milestone_json)
        return milestone

    def _github_request(self, endpoint, headers=None, params=None, follow_next=True, raw=False, prev_response=None):
        """
        Makes a request to a GHE endpoint, follows "next" links if there are additional results, using parameters
            contained in self to construct the request and evaluate rate limit complications appropriately.
        :param endpoint: The GHE REST endpoint to request data from.
        :param headers: Optional headers to send along with the request. Defaults to self.github_auth_header.
        :param params: Optional parameters to send along with the request.
        :param follow_next: Optional boolean to determine whether "next" links should be followed. Defaults to "True" so
            that the full range of the response can be processed.
        :param prev_response: Helper parameter that allows this to be joined with a set of previous response content.
            Intended for use with the follow parameter for building a single set of response return data.
        :return:
        """
        if not self.github:
            return None, None, None
        headers = headers or self.github_auth_header
        response = requests.get(endpoint, headers=headers, params=params)
        if self._retry_for_github_limits(response):
            return self._github_request(endpoint, headers, params, follow_next, prev_response)
        if prev_response:
            if not raw:
                prev_response += response.json()
            else:
                prev_response += response.content
        else:
            if not raw:
                prev_response = response.json()
            else:
                prev_response = response.content
        link_header = response.headers.get("Link")
        if follow_next and link_header is not None and not raw:
            links = link_header.split(", ")
            for link in links:
                if link.endswith('rel="next"'):
                    next_url = link[1:len(link) - 13]
                    return self._github_request(next_url, headers=headers, params=params, follow_next=follow_next, raw=False, prev_response=prev_response)
        return prev_response, response.headers, response.status_code

    def _github_post(self, endpoint, post_body, headers=None, params=None):
        if not self.github:
            return None, None, None
        headers = headers or self.github_auth_header
        response = requests.post(endpoint, json=post_body, headers=headers, params=params)
        if self._retry_for_github_limits(response):
            return self._github_post(endpoint, post_body, headers, params)
        return response.json(), response.headers, response.status_code

    def _github_patch(self, endpoint, post_body, headers=None, params=None):
        if not self.github:
            return None, None, None
        headers = headers or self.github_auth_header
        response = requests.patch(endpoint, json=post_body, headers=headers, params=params)
        if self._retry_for_github_limits(response):
            return self._github_patch(endpoint, post_body, headers, params)
        return response.json(), response.headers, response.status_code

    def _retry_for_github_limits(self, response):
        """

        :param response:
        :return:
        """
        if response.headers.get("X-RateLimit-Limit") is not None:
            self.github_limit = response.headers.get("X-RateLimit-Limit")
        if response.headers.get("X-RateLimit-Remaining") is not None:
            self.github_remaining = response.headers.get("X-RateLimit-Remaining")
        if response.headers.get("X-RateLimit-Reset") is not None:
            self.github_reset = response.headers.get("X-RateLimit-Reset")
        if response.status_code == requests.codes.forbidden and self.github_remaining <= 0:
            if self._github_limit_wait and int(time.time()) + 1 < self.github_reset:
                print("INFO: GHE rate limit reached. Waiting {} to retry.".format(int(self.github_reset - time.time())))
                time.sleep(self.github_reset - time.time() + 1)
                return True
            elif int(time.time()) + self._auto_retry_wait < self.github_reset:
                time.sleep(self.github_reset - time.time() + 1)
                return True
            else:
                raise IOError("GitHub rate limit exceeded. Rate will reset in {}. Retry operation after that time."
                              .format(self.github_reset - time.time()))
        try:
            response.raise_for_status()
        except BaseException as err:
            print(response.content)
            raise err
        return False

    def _zenhub_request(self, endpoint, headers=None, params=None):
        """
        Makes a request to a ZHE endpoint, using parameters contained in self to construct the request and evaluate
            rate limit complications appropriately. No capacity to follow large result sets, since ZHE doesn't seem
            to create those in our use cases.
        :param endpoint: The ZHE REST endpoint to request data from.
        :param headers: Optional headers to send along with the request. Defaults to self.zenhub_auth_header.
        :param params: Optional parameters to send along with the request. Defaults to self.github_issue_params.
        :return:
        """
        if not self.zenhub:
            return None, None, None
        headers = headers or self.zenhub_auth_header
        params = params or {}
        response = requests.get(endpoint, headers=headers, params=params)
        if self._retry_for_zenhub_limits(response):
            return self._zenhub_request(endpoint, headers, params)
        return response.json(), response.headers, response.status_code

    def _retry_for_zenhub_limits(self, response):
        """

        :param response:
        :return:
        """
        if response.headers.get("X-RateLimit-Limit") is not None:
            self.zenhub_limit = response.headers.get("X-RateLimit-Limit")
        if response.headers.get("X-RateLimit-Used") is not None:
            self.zenhub_api_used = response.headers.get("X-RateLimit-Used")
        if response.headers.get("X-RateLimit-Reset") is not None:
            self.zenhub_reset = response.headers.get("X-RateLimit-Reset")
        if response.status_code == requests.codes.forbidden and self.zenhub_limit >= self.zenhub_api_used:
            if self._zenhub_limit_wait and int(time.time()) + 1 < self.zenhub_reset:
                print("INFO: ZenHub rate limit reached. Waiting {} to retry.".format(int(self.zenhub_reset - time.time())))
                time.sleep(self.zenhub_reset - time.time() + 1)
                return True
            elif int(time.time()) + self._auto_retry_wait < self.zenhub_reset:
                time.sleep(self.zenhub_reset - time.time() + 1)
                return True
            else:
                raise IOError("GitHub rate limit exceeded. Rate will reset in {}. Retry operation after that time."
                              .format(self.zenhub_reset - time.time()))

        response.raise_for_status()
        return False


if __name__ == '__main__':
    raise NotImplementedError()
