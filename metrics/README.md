# F Prime Metrics Reporting
F Prime Metrics Reporting is a set of python scripts to generate report visualizations from various project metrics 
including GitHub issues, source lines of code, and component structures. The visualizations are referenced in a 
generated markdown file, suitable for storing in scm like GitHub or local viewing. The visualizations can also be added 
directly to other user generated reports. 

The scripts can store current sloc and component totals as an ongoing history, allowing trends to be viewed as they 
develop over time. They can also track your progress against a user defined plan. The progress is determined by matching 
tasks and gates in a user specified csv with closed issues on GitHub. A validator can display which items from the plan 
cannot be matched to GitHub issues. 

## Installation and Setup

1. Retrieve the scripts, and make a note of their location so they can be executed later.
1. Make a copy of the `config-general` and `plan-general.csv` files in the scripts directory for each of your deployments 
and targets; it is a good idea to name these something unique and clear for your deployment. These files should be saved 
in the location where you wish to store the generated the metrics artifacts.
    * If you wish the save the metrics and reports in GitHub for easy retrieval, we recommend a separate GitHub 
    repository from your main source code.
1. Modify the config file as appropriate for the deployment. Details for these options appear below.
    * A separate configuration file is currently required for each deployment or target. This will also mean a separate 
    execution of the script for each deployment or target.
1. Modify plan file as appropriate for the deployment effort.
    * Multiple plan files may be specified for a deployment, allowing you to display progress against a current plan, 
    previous plans, and proposed future plans simultaneously. The name of the plan file will be used as a key in the 
    chart's legend.
    
Here is an example of a configured directory setup:
```
-- deployment-metrics/
   -- plans/
      -- OriginalPlan
      -- RevisedPlan 
   -- deployment-config
```

## Requirements

**Python modules** 

Recent versions of these modules should be sufficient. Note that this is known to fail with matplotlib 1.3.1 
1. Requests
1. Matplotlib
 
**Software Metrics**

These scripts expect that you have created a ComponentReport and SlocReport file with the F Prime make commands, and 
then stored them in the deployment directory of the associated GitHub repo. 

The commands below will create the files, and should be executed in the appropriate deployment subdirectory. 
**Note** The SlocReport file must be manually redirected to the appropriate file at this time.

* `make <build>_sloc_csv > <build>_SlocReport.csv` 
* `make comp_report_gen`

**GitHub API Key**

You must use a GitHub API key to allow these scripts access to your GitHub repo. 

## Metrics Report Execution

The scripts are executed directly.

`python path/to/metricsreport.py --config path/to/config/<config_file>`

If your config file uses relative paths, this command should be executed from the common head of those paths. In the 
directory example above, you would launch this command from the `deployment-metrics` directory.

The following additional options are available for this command:
* `--github-api-key <key>` - Used when you do not wish to store the api key in the config file. A good choice when
using CI to automate the generation.
* `--show` - Causes visualizations to be rendered on the screen rather than saved to a file. Will cause the report
to be not generated, but history and other secondary artifacts may be updated. 

## Config File Options

These fields are present in the config-general file stored with the scripts. Required and strongly recommended Optional
fields are uncommented. Additional comments can be added to the file by starting the line with a `#`.

| Parameter | Format | Required | Description |
| --- | --- | --- | --- |
| github_base_url | String | Required | The base url endpoint for the GitHub server hosting the repository. For public GitHub, this should be `https://api.github.com`. |
| github_api_key | String | Optional | A GitHub API key for an account that has read access to the issues in the repository. Generated according to directions at https://help.github.com/articles/creating-a-personal-access-token-for-the-command-line/. If not specified in the config file, this must be passed in from command line (as shown in examples above).|
| github_repo_list | List of Strings with format "owner/repo" | Required | A list of the repos that contain the GitHub issues to be evaluated, as well as the the repo that contains the SlocReport and ComponentReport files. Submodules that do not contain desired issues do not need to be added; they submodule links will be traversed automatically.|
| github_branch | String | Optional | Name of branch to query in repos. Will be used for all repos in above list. Defaults to "master". |
| github_issue_parameters | List of Strings with format "parameter:value" | Optional | Used by the GitHub API to determine which issues should be returned. Recommended value is `state:all`, which causes open and closed issues to be included in the reporting. Defaults to open issues only. Additional information and options at `https://developer.github.com/v3/issues/`. |
| github_label_mappings | List of String with format "OldLabelName:NewLabelName" | Optional | Mapping of original label names to current label names. Needed because GitHub does not update historical labeling events, and this can lead to name mismatches. | 
| included_labels | List of Strings | Optional | GitHub issue label names to be included in the reporting. A value of "*" causes all issues to be included unless they have a label whose name appears in the excluded_tags parameter. Defaults to "*". |
| excluded_labels | List of Strings | Optional | GitHub issue label names to be excluded from the reporting. Issues with a label listed here will NOT be included in the metrics or planning report, even if other labels are present on the issue.|
| metrics_make_deployment | String | Optional* | Name of deployment. Script will attempt to infer component directories from the make file if this name is specified. This is the preferred way of determining components to include in reporting. |
| metrics_make_location | String | Optional | Location of the modules make file where a specified make deployment name can be found. Not referenced when metrics_make_deployment is not specified. Defaults to "mk/configs/modules/modules.mk" |
| metrics_components_list | List of Strings with format "component:directory" OR "directory" | Optional* | If metrics_make_deployment is not specified, this field must be used to indicate the list of directories to include. The component mapping may be used to alias directories. This is compatible with the metrics_make_deployment option, and will alias directories found there is specified here. |  
| metrics_build_prefix | String | Optional | The target of a build, which is prefixed to the SlocReport for the target. Example: "LINUX_". Required when a deployment is intended to be built on multiple target platforms. |
| metrics_sloc_estimation | Integer | Optional | Estimate of total SLOC required for deployment. Only used in the Component SLOC Snapshot chart. |
| metrics_sloc_types | List of Strings | Optional | The types of SLOC to display in the metrics. Defaults to "HAND, AC, XML" as created in the SlocReport. |
| metrics_comp_types | List of Strings | Optional | The structures of the components to display in the metrics. Defaults to "Channels, Commands, Events, Parameters, Total Ports" as created in the ComponentReport. |
| metrics_periods | List Integers | Optional | The day ranges to display in the various diff tables. The maximum value is used to limit some charts. Defaults to "60, 30, 14, 7". |
| metrics_sloc_history | String | Optional* | Path to the sloc history location within the repo. The file will be created if it does not exist. If not specified, sloc history reports will fail and no history will be appended from the current execution. Example: "report/slochistory.csv". |
| metrics_comp_history | String | Optional* | Path to the sloc history location within the repo. The file will be created if it does not exist. If not specified, comp history reports will fail and no history will be appended from the current execution. Example: "report/comphistory.csv". |
| metrics_ev_plan_files | List of Strings | Optional | List of paths to plan files to be included in the reporting. The first plan file in the list will be used as the primary plan, and will be used to determine earned value from closed issues. |
| metrics_report_filename | String | Optional | Name of markdown file for the metrics to generate. Defaults to "README.md", since that will be automatically displayed by GitHub during repo browsing. | 
| metrics_report_dir | String | Optional* | Path to the directory where the report file should be created. Defaults to directory where the script is executed. |
| metrics_report_artifact_dir | String | Optional | Path to the directory where the visualizations for the report should be created. Must be a subdirectory of the report directory specified above. Defaults to "image/". |
| metrics_report_sections | List of Strings | Optional* | List of the methods to execute to generate visualizations. The mapping between the visualization names and the needed entries in this list appears below. If not included, no report will be generated but other history metrics may be updated. |   

### Metrics Report Section Mappings
The following visualizations can be created by the metrics report script. 

| Visualization Name | Config Entry | Notes |
| :--- | :--- | :--- |
| Active Task Progress | bar_chart_active_tasks |  |
| Component SLOC | bar_plot_sloc | All SLOC types for each component are shown side by side in a single bar plot. Not recommended for large numbers of components. |
| Component SLOC | split_bar_plot_sloc | Each SLOC type is shown on a separate bar plots. Recommended for large numbers of components, though can grow quite wide. |
| Component SLOC Changes | table_sloc_diffs |  |
| Component SLOC Comparison | stacked_bar_plot_sloc_compare |  |
| Component SLOC Overview | pie_plot_sloc_overview |  |
| Component SLOC Snapshot | bar_plot_sloc_snapshot |  |
| Component SLOC Summary | table_sloc_summary |  |
| Component SLOC Totals | pie_plot_sloc_totals |  |
| Component SLOC Trend | stacked_bar_plot_sloc_trend |  |
| Component SLOC Trendline | line_plot_sloc_trend |  |
| Component Structure | bar_plot_comp | All component types for each component are shown side by side in a single bar plot. Not recommended for large numbers of components. |
| Component Structure | split_bar_plot_comp | Each component type is shown on a separate bar plots. Recommended for large numbers of components, though can grow quite wide. |
| Component Structure Changes | table_comp_diffs |  |
| Component Structure Comparison | stacked_bar_plot_comp_compare |  |
| Component Structure Overview | pie_plot_comp_overview |  |
| Component Structure Summary | table_comp_summary |  |
| Component Structure Totals | pie_plot_comp_totals |  |
| Component Structure Trend | stacked_bar_plot_comp_trend |  |
| Component Structure Trendline | line_plot_comp_trend |  |
| Issue Label Changes | table_issue_label_diffs |  |
| Issue Label Summary | table_issue_label_summary |  |
| Issue Label Totals | pie_plot_issue_labels_totals |  |
| Issue Label Trend | stacked_bar_plot_issue_labels_trend |  |
| Issue Label Trendline | line_plot_issue_labels_trend |  |
| Planned Task List | table_task_list |  |
| Project Changes | table_project_diffs |  |
| Project Summary | table_project_summary |  |
| Task Progress vs Plan | line_plot_ev_plan |  |

## Plan Files

Plan files are a simple csv format that support high level task tracking. They include entries for task name, assigned 
engineer, release milestone, planned value, scheduled start, scheduled end, and a variable number of gates to break
a high level large task into more discrete portions. For example, a task line might be the following: 

`FlightController,User1,Release1,10,2019-01-01,2019-06-01,Design,20,Implementation,90,Test,100`

The script will attempt to match each task in a line with one or more GitHub issues that either start with the task 
name and a colon, or have a label that matches the task name. If gates are specified for the task, the script will look 
for an issue that matches the preceding constraint and also has a label that matches the gate name. In the above 
example the script will look for three GitHub issues whose name starts with "FlightController" OR that has a 
"FlightController" label and also has a "Design", "Implementation", or "Test" labels. When one of these three issues is 
closed, the specified percentage of earned value for that gate will be added to the plan progress metric.

The script takes an all or none approach to planned value. If a task is set to end on a date, then the user will see a
jump in the expectation value on that date rather than a gradual slope upwards to it from the start date. The start date
is used when multiple gates are specified, like in the above example, as a way to determine when the percentages for 
each gate should be added to the expectation value. Continuing the above example, 2 would be added to the expectation 
value 20% of the way through the period, another 7 added 90% of the way through the period, and the final 1 would be 
on the due date.

Dates for this file should be added according to the ISO 8601 standard: YYYY-MM-DD. Alternate date formats are
not supported. 

Percentages for the gates should be added as cumulative values, not individual contributions. They may be added as 
either integers going to 100 or as floats going to 1.0.

The name for the plan file will be used in the legend of the plan charts.

## Plan Validator

The planvalidator.py script is used as part of the metrics process, but can also be executed as a standalone script. In
standalone mode, it verifies that every task/gate combination in your plan has a corresponding GitHub issue. If
additional flags are passed to the script, it can also create new GitHub objects to represent your plan, or update
existing ones to reflect a new plan state.

In the event that the config file specified multiple GitHub repos or multiple plans, the first of each is used for the 
validation.

### Execution  

The script is executed directly.

`python path/to/planvalidator.py --config path/to/config/<config_file>`

If your config file uses relative paths, this command should be executed from the common head of those paths. In the 
directory example above, you would launch this command from the `deployment-metrics` directory.

The following additional options are available for this command:
* `--github-api-key <key>` - Used when you do not wish to store the api key in the config file. A good choice when
using CI to automate the generation.
* `--show_found` - Causes the validator to display found issues, whether the details match or are different.
*`--create` - Causes the validator to create GitHub objects for missing gates, releases, and tasks.
*`--update` - Causes the validator to update GitHub objects such that they match their representation in the plan.
*`--task-labels` - Used in conjunction with the `--create` option. Causes the validator to create a label for the task
name as well as the gate.

### Use cases

The two primary use cases for this script are to create a new set of GitHub issues for a new plan, or to update a set 
of GitHub issues for a revised plan. Both cases follow the same suggested flow:

1. Create or modify a plan file, and save it as the first entry in the configuration file.
1. Run `python path/to/planvalidator.py --config path/to/config/<config_file> --show-found` to verify the current
state of the GitHub repository.
    * If a label or release appears in the missing list that does not seem right, you likely have a typo in your plan
    and should correct it so that an incorrect object isn't created.
    * Double check that the missing and found items make sense. You can still manually adjust objects in the GitHub
    repo if something was not found that should have been, or vice versa. 
    * Rerun the above command if needed to verify.
1. Run `python path/to/planvalidator.py --config path/to/config/<config_file> --show-found --create --update` to create
and update the GitHub items.
    * You can omit the `--create` to only update GitHub issues, but any issue that relies on a missing label or 
    milestone will fail.
    * If a creation fails because of a bad plan entry (i.e. not using the GitHub login for the assignee field), you may
    correct the field and retry the above command to update the GitHub entry.  