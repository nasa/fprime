import requests
import time


class GitHubConnector:
    """
    Helper class for performing various GHE and ZHE operations.

    Note that this class caches GHE objects locally, so only the first call will use GHE resources in most
    circumstances. To force a re-acquisition of the GHE object, empty the appropriate self.<type>[owner/repo]
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

    def __init__(self, config_options, github_limit_wait=False, zenhub_limit_wait=False, auto_retry_wait=10000):
        """
        Initializer for the gheconnector.GitHubConnector object.
        :param config_options: A reportconfig.ReportConfig object that contains a number of parameters to identify the
            GHE and ZHE servers.
        :param github_limit_wait: If true, request will abort when rate limit reached. If false, request will wait
            until limit is reset and then continue. Defaults to "True", because the GHE reset time is long.
        :param zenhub_limit_wait: If true, request will abort when rate limit reached. If false, request will wait
            until limit is reset and then continue. Defaults to "False", because the ZHE reset time is short and we
            can generally afford it.
        :param auto_retry_wait: A number of ms to wait for the rate limit to reset, regardless of other settings. Don't
            want to abort when it'll reset shortly. Defaults to 10s.
        """
        self._github_limit_wait = github_limit_wait
        self._zenhub_limit_wait = zenhub_limit_wait
        self._auto_retry_wait = auto_retry_wait

        self.github_url = config_options.git_base_url
        self.github_key = config_options.git_api_key
        self.found_submodules = {}

        if self.github_url is None or self.github_url == '' or self.github_key is None or self.github_key == '':
            self.github = False
        else:
            self.github = True
            self.github_auth_header = {"Authorization": "token " + config_options.git_api_key}
            self.github_issue_params = {}
            for param in config_options.git_params_list:
                entry = param.split(":")
                self.github_issue_params[entry[0]] = entry[1]
            self.github_limit = 1
            self.github_remaining = 1
            self.github_reset = 0
            self.repos = {}
            self.issues = {}
            self.labels = {}
            self.milestones = {}

        self.zenhub_url = config_options.zen_base_url
        self.zenhub_key = config_options.zen_api_key
        if self.zenhub_url is None or self.zenhub_url == '' or self.zenhub_key is None or self.zenhub_key == '':
            self.zenhub = False
        else:
            self.zenhub = True
            self.zenhub_auth_header = {"X-Authentication-Token": config_options.zen_api_key}
            self.zenhub_limit = 1
            self.zenhub_api_used = 0
            self.zenhub_reset = 0

    def get_raw_file(self, repo_name, file_path, ref="master", traverse_submodules=False):
        """
        Light wrapper around the requests library to retrieve a raw file from GHE. Doesn't call the self._github_request
            method because it doesn't receive a json response, and it doesn't need to because it doesn't count against
            api calls.
        :param repo_name: GHE repository name, including owner. ex: "organization/repository"
        :param ref: Branch of the repository to look in.
        :param file_path: Relative path to the file from the root of the repository. Requires Linux/URL like file paths.
        :param traverse_submodules:
        :return: Tuple consisting of: response content (raw file) AND response headers AND response status code.
            A status code != 200 means that the file does not exist or is not accessible (i.e. forbidden).
        """
        if not self.github:
            return None, None, None
        c, h, s = self._get_contents(repo_name, ref=ref, path=file_path, traverse_submodules=traverse_submodules)
        if s != 200:
            return None, None, None
        github_file_url = c["download_url"]
        response = requests.get(github_file_url, headers=self.github_auth_header)
        return response.content, response.headers, response.status_code

    def get_repo(self, repo):
        """

        :param repo:
        :return:
        """
        if repo not in self.repos.keys():
            github_repo_url = self.github_url + self.API_ENDPOINT + '/' + repo
            github_repo_data, github_repo_headers, status = self._github_request(github_repo_url)
            if status != requests.codes.ok:
                raise IOError("Unable to retrieve repo " + repo + " : " + str(status))
            self.repos[repo] = github_repo_data
        return self.repos[repo]

    def get_submodule_repo(self, repo_name, path, ref="master"):
        """
        Retrieves a submodule org/repo and sha ref from a submodule object in a repo. Verifies that it is grabbing a
            submodule object, so it can be used to differentiate if needed.
        :param repo_name: field to specify a repo directly to retrieve the submodule object from
        :param ref: field to specify a ref directly to retrieve the submodule object from
        :param path: field to specify a path directly for the location of the submodule object
        :return: Tuple consisting of "org/repo" string AND submodule commit ref,
            OR None And None if it is not a submodule.
        """
        if not self.github:
            return None, None
        github_contents_url = self.github_url + self.API_ENDPOINT + '/' + repo_name + self.CONTENTS_SUFFIX + path
        params = {"ref": ref}
        submodule_json, headers, status = self._github_request(github_contents_url, params=params, follow=False)
        if status != 200:
            return None, None
        file_type = submodule_json['type']
        if file_type is None or file_type != 'submodule':
            raise IOError("Element {} is not a submodule, but a {}.".format(submodule_json, file_type))
        submodule_repo = submodule_json['submodule_git_url'][len(self.github_url) + 1:-4]
        submodule_ref = submodule_json['sha']
        return submodule_repo, submodule_ref

    def _get_contents(self, repo_name, ref="master", path="", traverse_submodules=False):
        """
        Retrieves the directory contents from a GHE repo.
        :param repo_name: GHE repository name, including owner. ex: "organization/repository"
        :param ref: Branch / tag / ref of the repository to look in. Defaults to master.
        :param path: Relative path to the directory from the root of the repository. Expects Linux/URL like file
            paths. Defaults to top level of repository.
        :param traverse_submodules: Attempts to validate directory structure and traverse any discovered submodule
            links. Expensive, since it has to check each directory step, but found results will be cached.
        :return: Tuple consisting of: response content (json list) AND response headers AND response status code.
            A status code != 200 means that the directory does not exist or is not accessible (i.e. forbidden).
        """
        if not self.github:
            return None, None, None

        path = "/" + path

        unchecked = True
        while traverse_submodules and unchecked:
            unchecked = False
            for f_repo_name, f_ref, f_path in self.found_submodules.keys():
                if repo_name == f_repo_name and ref == f_ref and path.startswith(f_path):
                    repo_name, ref, f_path = self.found_submodules[(repo_name, ref, f_path)]
                    path = path[len(f_path) + 1:]
                    unchecked = True

        github_contents_url = self.github_url + self.API_ENDPOINT + '/' + repo_name + self.CONTENTS_SUFFIX + path
        params = {"ref": ref}
        c, h, s = self._github_request(github_contents_url, params=params, follow=False)
        if s == 404 and traverse_submodules:
            current_path = ""
            path_steps = path[1:].split('/')
            while path_steps:
                current_path += "/" + path_steps[0]
                del(path_steps[0])
                github_contents_url = self.github_url + self.API_ENDPOINT + '/' + repo_name + self.CONTENTS_SUFFIX + current_path
                params = {"ref": ref}
                c1, h1, s1 = self._github_request(github_contents_url, params=params, follow=False)
                if s1 != 200:
                    break
                try:
                    element_type = c1['type']
                    if element_type == "submodule":
                        submodule_repo = c1['submodule_git_url'][len(self.github_url) + 1:-4]
                        submodule_ref = c1['sha']
                        self.found_submodules[(repo_name, ref, current_path)] = (submodule_repo, submodule_ref, current_path)
                        repo_name = submodule_repo
                        ref = submodule_ref
                        current_path = ""
                except TypeError:
                    # it's either a list directory, or something else we don't want to process. update the returns and
                    # move on
                    pass
                c, h, s = c1, h1, s1
        return c, h, s

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
        if self.issues.get(repo_name) is None:
            self.issues[repo_name] = {}
        if not self.github:
            return self.issues[repo_name]
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
        github_issue_events_data, headers, status = self._github_request(github_issue_events_url)
        events += github_issue_events_data
        if self.zenhub and zenhub_issues_url:
            zenhub_issue_events_url = zenhub_issues_url + "/" + str(issue_number) + self.EVENTS_SUFFIX
            zenhub_issue_events_data, headers, status = self._zenhub_request(zenhub_issue_events_url)
            events += zenhub_issue_events_data
        return events

    def get_labels(self, repo_name):
        if not self.github:
            return {}
        if repo_name not in self.labels:
            self.labels[repo_name] = {}
        # /repos/:owner/:repo/labels
        github_label_url = self.github_url + self.API_ENDPOINT + '/' + repo_name + self.LABELS_SUFFIX
        github_label_data, github_label_headers, status = self._github_request(github_label_url)
        for label_json in github_label_data:
            label_name = label_json["name"]
            self.labels[repo_name][label_name] = label_json
        return self.labels[repo_name]

    def get_single_label(self, repo_name, label_name):
        if not self.github:
            return {}
        if repo_name not in self.labels:
            self.labels[repo_name] = {}
        if label_name not in self.labels.keys():
            # /repos/:owner/:repo/labels
            github_label_url = self.github_url + self.API_ENDPOINT + '/' + repo_name + self.LABELS_SUFFIX \
                               + "/" + label_name
            github_label_data, github_label_headers, status = self._github_request(github_label_url)
            self.labels[repo_name][label_name] = github_label_data
        return self.labels[repo_name][label_name]

    def get_milestones(self, repo_name):
        if not self.github:
            return {}
        if repo_name not in self.milestones:
            self.milestones[repo_name] = {}
        # /repos/:owner/:repo/labels
        github_milestone_url = self.github_url + self.API_ENDPOINT + '/' + repo_name + self.MILESTONES_SUFFIX
        github_milestone_data, github_milestone_headers, status = self._github_request(github_milestone_url)
        for milestone_json in github_milestone_data:
            milestone_number = milestone_json["number"]
            self.milestones[repo_name][milestone_number] = milestone_json
        return self.milestones[repo_name]

    def get_single_milestone(self, repo_name, milestone_number):
        if not self.github:
            return {}
        if repo_name not in self.milestones:
            self.milestones[repo_name] = {}
        if milestone_number not in self.milestones.keys():
            # /repos/:owner/:repo/labels
            github_milestone_url = self.github_url + self.API_ENDPOINT + '/' + repo_name + self.MILESTONES_SUFFIX \
                                   + "/" + milestone_number
            github_milestone_data, github_milestone_headers, status = self._github_request(github_milestone_url)
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
        github_issue_data, github_issue_headers, status = self._github_post(github_issues_url, issue_json)
        issue_number = github_issue_data["number"]
        if repo_name not in self.issues.keys():
            self.issues[repo_name] = {}
        self.issues[repo_name][issue_number] = github_issue_data

    def create_label(self, repo_name, label_name, label_color, description=None):
        """

        :param repo_name:
        :param label_name:
        :param label_color:
        :param description:
        :return:
        """
        label_json = {"name": label_name, "color": label_color}
        if description:
            label_json["description"] = description
        github_repo_url = self.github_url + self.API_ENDPOINT + '/' + repo_name
        github_labels_url = github_repo_url + self.LABELS_SUFFIX
        github_issue_data, github_issue_headers, status = self._github_post(github_labels_url, label_json)
        if repo_name not in self.labels.keys():
            self.labels[repo_name] = {}
        self.labels[repo_name][label_name] = github_issue_data

    def create_milestone(self, repo_name, milestone_name, state=None, description=None, due_date=None):
        """

        :param repo_name:
        :param milestone_name:
        :param state:
        :param description:
        :param due_date:
        :return:
        """
        milestone_json = {"title": milestone_name}
        if state:
            milestone_json["state"] = state
        if description:
            milestone_json["description"] = description
        if due_date:
            if len(due_date) == 10:
                due_date += "T08:00:00Z"
            milestone_json["due_on"] = due_date
        github_repo_url = self.github_url + self.API_ENDPOINT + '/' + repo_name
        github_milestones_url = github_repo_url + self.MILESTONES_SUFFIX
        github_issue_data, github_issue_headers, status = self._github_post(github_milestones_url, milestone_json)
        milestone_number = github_issue_data["number"]
        if repo_name not in self.milestones.keys():
            self.milestones[repo_name] = {}
        self.milestones[repo_name][milestone_number] = github_issue_data

    def _github_post(self, endpoint, post_body, headers=None, params=None):
        if not self.github:
            return None, None, None
        headers = self.github_auth_header if headers is None else headers
        # params = {} if params is None else params
        response = requests.post(endpoint, json=post_body, headers=headers, params=params)
        if self._process_github_limits(response):
            return self._github_post(endpoint, post_body, headers, params)
        return response.json(), response.headers, response.status_code

    def _github_request(self, endpoint, headers=None, params=None, follow=True, prev_response=None):
        """
        Makes a request to a GHE endpoint, follows "next" links if there are additional results, using parameters
            contained in self to construct the request and evaluate rate limit complications appropriately.
        :param endpoint: The GHE REST endpoint to request data from.
        :param headers: Optional headers to send along with the request. Defaults to self.github_auth_header.
        :param params: Optional parameters to send along with the request.
        :param follow: Optional boolean to determine whether "next" links should be followed. Defaults to "True" so
            that the full range of the response can be processed.
        :param prev_response: Helper parameter that allows this to be joined with a set of previous response content.
            Intended for use with the follow parameter for building a single set of response return data.
        :return:
        """
        if not self.github:
            return None, None, None
        if self.github_remaining <= 0 and int(time.time()) + 1 < self.github_reset:
            if self._github_limit_wait:
                raise IOError("Github request limit reached, abort requested.")
            time.sleep(self.github_reset - time.time() + 1)
        headers = self.github_auth_header if headers is None else headers
        # params = {} if params is None else params
        response = requests.get(endpoint, headers=headers, params=params)
        if self._process_github_limits(response):
            return self._github_request(endpoint, headers, params, follow, prev_response)
        if prev_response is not None:
            prev_response += response.json()
        else:
            prev_response = response.json()
        link_header = response.headers.get("Link")
        if follow and link_header is not None:
            links = link_header.split(" ")
            if links[1] == "rel=\"next\",":
                next_link = links[0]
                next_url = next_link[1:len(next_link) - 2]
                return self._github_request(next_url, headers, params, True, prev_response)
        return prev_response, response.headers, response.status_code

    def _process_github_limits(self, response):
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
        response.raise_for_status()
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
        headers = self.zenhub_auth_header if headers is None else headers
        params = {} if params is None else params
        response = requests.get(endpoint, headers=headers, params=params)
        if self._process_zenhub_limits(response):
            return self._zenhub_request(endpoint, headers, params)
        return response.json(), response.headers, response.status_code

    def _process_zenhub_limits(self, response):
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
        response.raise_for_status()
        return False


if __name__ == '__main__':
    raise NotImplementedError()
