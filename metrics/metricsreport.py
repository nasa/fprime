import sys
import datetime
import reportconfig
import projectmetrics
import os
import numpy as np
import matplotlib
import matplotlib.dates as mdates

# check for headless executions
if "DISPLAY" not in os.environ:
    if os.system('python -c "import matplotlib.pyplot as plt; plt.figure()"') != 0:
        print("INFO: Lack of display should generate an expected ImportError. Changing MatPlotLib backend.")
        matplotlib.use('Agg')
    import matplotlib.pyplot as plt
else:
    import matplotlib.pyplot as plt


# import constants for the days of the week
from matplotlib.dates import MO, TU, WE, TH, FR, SA

plt.style.use('ggplot')
# plt.style.use('fivethirtyeight')
# plt.style.use('classic')
# plt.style.use('seaborn')

YEARS = mdates.YearLocator()  # every year
MONTHS = mdates.MonthLocator()  # every month
WEEKDAYS = mdates.WeekdayLocator(byweekday=(MO, TU, WE, TH, FR))  # every weekday
WEEKFINISH = mdates.WeekdayLocator(byweekday=SA)  # every week start
YEARS_FORMAT = mdates.DateFormatter('%Y')
MONTHS_FORMAT = mdates.DateFormatter('%b %Y')

# DEFAULT_CMAP = "Set2"
# DEFAULT_CMAP = "Set3"
# DEFAULT_CMAP = "prism"
DEFAULT_CMAP = "tab10"
SECONDARY_CMAP = "gist_ncar"

DEFAULT_TREND_RANGE = [60, 30, 14, 7]
DEFAULT_SLOC_TYPES = ["HAND", "AC", "XML"]
DEFAULT_COMP_TYPES = ["Channels", "Commands", "Events", "Parameters", "Total Ports"]
DEFAULT_ISSUE_LABELS = ["Bug", "Req. Change", "Enhancement", "Process", "Issue"]

DEFAULT_BAR_WIDTH = 0.8

I = 'issues'
S = 'sloc'
C = 'comp'


class GitHubMetricsReport:

    def __init__(self, args):
        if "--config" in args:
            config_file = args[args.index("--config") + 1]
        else:
            config_file = args[2]
        self.config_opts = reportconfig.ReportConfiguration(config_file)

        if "--username" in args:
            self.config_opts.username = args[args.index("--username") + 1]

        if "--git-api-key" in args:
            self.config_opts.git_api_key = args[args.index("--git-api-key") + 1]

        if "--zen-api-key" in args:
            self.config_opts.zen_api_key = args[args.index("--zen-api-key") + 1]

        if "--show" in args:
            self.show = True
        else:
            self.show = False

        self.metrics = projectmetrics.ProjectMetrics(None, config_opts=self.config_opts)


def create_graph_colors_list(data_types):
    if len(data_types) > 20:
        cmap = plt.get_cmap(SECONDARY_CMAP)
        colors_list = cmap(np.linspace(0., 1., len(data_types)))
    else:
        cmap = plt.get_cmap(DEFAULT_CMAP)
        colors_list = cmap(np.arange(len(data_types)))
    return colors_list


def format_label_chart(fig, axs, x_data):
    try:
        for ax in axs:
            ax.legend()
            ax.set_xticks(np.array(list(range(len(x_data)))))
            ax.set_xticklabels(x_data, rotation=90)
            x_lim = ax.get_xlim()
            ax.set_xlim(-1, len(x_data))
            y_lim = ax.get_ylim()
            ax.set_ylim(y_lim[0], 1.05 * y_lim[1])
    except TypeError:
        axs.legend()
        axs.set_xticks(np.array(list(range(len(x_data)))))
        axs.set_xticklabels(x_data, rotation=90)
        x_lim = axs.get_xlim()
        axs.set_xlim(-1, len(x_data))
        y_lim = axs.get_ylim()
        axs.set_ylim(y_lim[0], 1.05*y_lim[1])
    fig.tight_layout()
    return fig


def format_date_chart(fig, axs, x_data):
    try:
        for ax in axs:
            ax.xaxis_date()
            ax.legend()
            ax.xaxis.set_major_locator(MONTHS)
            ax.xaxis.set_major_formatter(MONTHS_FORMAT)
            y_lim = ax.get_ylim()
            ax.set_ylim(y_lim[0], 1.05 * y_lim[1])
            # if len(data_x) <= 120:
            #     ax.xaxis.set_minor_locator(WEEKDAYS)
            # else:
            #     ax.xaxis.set_minor_locator(WEEKFINISH)
    except TypeError:
        axs.xaxis_date()
        axs.legend()
        axs.xaxis.set_major_locator(MONTHS)
        axs.xaxis.set_major_formatter(MONTHS_FORMAT)
        y_lim = axs.get_ylim()
        axs.set_ylim(y_lim[0], 1.05*y_lim[1])
        # if len(data_x) <= 120:
        #     axs.xaxis.set_minor_locator(WEEKDAYS)
        # else:
        #     axs.xaxis.set_minor_locator(WEEKFINISH)
    fig.autofmt_xdate()
    fig.tight_layout()
    return fig


def finalize_figure(fig, title, directory=None, show=False):
    if show:
        plt.show()
        plt.close(fig)
        return
    if directory is not None:
        output_file = directory + title + ".png"
        output_file = output_file.replace(" ", "_")
        plt.savefig(output_file)
        plt.close(fig)
        return output_file


def generate_table(table_columns, data, title="", directory=None, show=False):
    fig, ax = plt.subplots(1, 1, figsize=(10, (len(data) + 2) / 4 + 1))
    # fig.patch.set_visible(False)
    ax.axis('off')
    table = ax.table(cellText=data, colLabels=table_columns, loc='center')
    for index, header in enumerate(table_columns):
        table.auto_set_column_width(index)
    table.auto_set_font_size(True)
    ax.set_title(title)

    fig.tight_layout()

    output_file = finalize_figure(fig, title, directory, show)
    return output_file


def generate_line_plot(x_data, y_data, filled=None, data_labels=None, title="", directory=None, show=False,
                       date_plot=False, stacked=False):
    if data_labels is None:
        data_labels = list(y_data.keys())

    if date_plot:
        x_index = x_data
    else:
        x_index = np.array(list(range(len(x_data))))
    y_offset = np.zeros((len(x_index),))

    colors = create_graph_colors_list(data_labels)

    fig, ax = plt.subplots(1, 1, figsize=(10, 10))
    for index, label in enumerate(data_labels):
        if isinstance(x_data, dict):
            if stacked:
                raise ValueError("Stacked line charts require shared x_data basis.")
            x = x_data[label]
            y_offset = np.zeros((len(x),))
        else:
            x = x_index
        y = y_data[label]
        if stacked:
            y += y_offset
        if date_plot:
            ax.plot_date(x, y, '-', color=colors[index], label=label)
        else:
            ax.plot(x, y, '-', color=colors[index], label=label)
        if filled and label in filled:
            ax.fill_between(x, y, y_offset, color=colors[index], alpha=0.4)
        if stacked:
            y_offset += y
    ax.set_title(title)

    # format the ticks
    if date_plot:
        format_date_chart(fig, ax, x_data)
    else:
        format_label_chart(fig, ax, x_data)

    # handles, labels = _sort_legend(ax)
    # ax.legend(handles, labels)
    output_file = finalize_figure(fig, title, directory, show)
    return output_file


def _generate_complicated_bar_plot(x_data, y_data, data_labels=None, title="", directory=None, show=False,
                                   date_plot=False, split=False, adjacent=False, stacked=False):
    if data_labels is None:
        data_labels = list(y_data.keys())

    bar_width = DEFAULT_BAR_WIDTH
    colors = create_graph_colors_list(data_labels)

    if date_plot:
        # TODO consider re-enabling; expand chart when range > 60 days
        # sorted_x_data = sorted(x_data)
        # fig_x = max(10., ((sorted_x_data[-1] - sorted_x_data[0]).days + 1) / 6.)
        fig_x = 10
    else:
        # expand chart when components > 25
        fig_x = max(10., len(x_data) / 2.5)
    if split and len(data_labels) > 1:
        fig, axs = plt.subplots(len(data_labels), 1, figsize=(fig_x, 5 * len(data_labels)))
        ax = axs[0]
    else:
        axs = []
        fig, ax = plt.subplots(1, 1, figsize=(fig_x, 10))

    if date_plot:
        x = x_data
    else:
        x = np.array(list(range(len(x_data))))
        if adjacent:
            bar_width /= len(data_labels)
            x = x - (len(data_labels) - 1) * bar_width / 2
    y_offset = np.zeros((len(x),))

    for index, label in enumerate(data_labels):
        if isinstance(x_data, dict):
            if stacked:
                raise ValueError("Stacked line charts require shared x_data basis.")
            x = x_data[label]
            y_offset = np.zeros((len(x),))
        if split and len(data_labels) > 1:
            ax = axs[index]
        y = y_data[label]
        bars = ax.bar(x, y, width=bar_width, bottom=y_offset, color=colors[index], label=label)
        if not date_plot:
            if adjacent:
                x = x + bar_width
            for position, bar in enumerate(bars):
                height = bar.get_height()
                if height != 0:
                    ax.text(bar.get_x() + bar.get_width() / 2., height + y_offset[position], " {} ".format(height),
                            ha='center', va='bottom')
                    # ha='center', va='bottom', rotation=90)
        if stacked:
            y_offset = y_offset + y_data[label]
        if index == 0:
            ax.set_title(title)

    if split:
        ax = axs
    if date_plot:
        format_date_chart(fig, ax, x_data)
    else:
        format_label_chart(fig, ax, x_data)

    output_file = finalize_figure(fig, title, directory, show)
    return output_file


def generate_stacked_bar_plot(x_data, y_data, data_labels=None, title="", directory=None, show=False, date_plot=False):
    return _generate_complicated_bar_plot(x_data, y_data, data_labels, title, directory, show, date_plot, stacked=True)


def generate_split_bar_plot(x_data, y_data, data_labels=None, title="", directory=None, show=False, date_plot=False):
    return _generate_complicated_bar_plot(x_data, y_data, data_labels, title, directory, show, date_plot, split=True)


def generate_adjacent_bar_plot(x_data, y_data, data_labels=None, title="", directory=None, show=False, date_plot=False):
    return _generate_complicated_bar_plot(x_data, y_data, data_labels, title, directory, show, date_plot, adjacent=True)


def generate_pie_plot():
    raise NotImplementedError()


def generate_stacked_pie_plot():
    raise NotImplementedError()


def table_project_summary(reporter, categories=None, period=None, show=False, directory=None, title="Project Summary"):
    metrics = reporter.metrics

    table_columns = [""] + ["Current Value"]
    table_data = []

    # TODO evaluate issue label filter approach

    # issue label counts, starting with overall
    if categories[I]:
        total = metrics.issue_totals[metrics.OV][metrics.NEW][-1] - metrics.issue_totals[metrics.OV][metrics.DONE][-1]
        table_data.append([metrics.OV + " issues", total])
    for key in categories[I]:
        if key == metrics.OV:
            continue
        total = metrics.issue_totals[key][metrics.NEW][-1] - metrics.issue_totals[key][metrics.DONE][-1]
        table_data.append([key + " issues", total])

    # sloc
    for category in categories[S]:
        total = 0
        for key in (list(metrics.sloc_data.keys())):
            total += metrics.sloc_data[key].get(category) \
                if metrics.sloc_data[key].get(category) is not None else 0
        table_data.append([category, total])

    # component counts
    for comp in categories[C]:
        total = 0
        for key in list(metrics.comp_data.keys()):
            total += metrics.comp_data[key].get(comp) \
                if metrics.comp_data[key].get(comp) is not None else 0
        table_data.append([comp, total])

    output_file = generate_table(table_columns, table_data, title, directory, show)
    return output_file


def table_issue_label_summary(reporter, categories=None, period=None, show=False, directory=None, title="Issue Label Summary"):
    categories = {I: categories[I], S: [], C: []}
    return table_project_summary(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def table_sloc_summary(reporter, categories=None, period=None, show=False, directory=None, title="Component SLOC Summary"):
    categories = {I: [], S: categories[S], C: []}
    return table_project_summary(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def table_comp_summary(reporter, categories=None, period=None, show=False, directory=None, title="Component Structure Summary"):
    categories = {I: [], S: [], C: categories[C]}
    return table_project_summary(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def table_project_diffs(reporter, categories=None, period=None, show=False, directory=None, title="Project Changes"):
    metrics = reporter.metrics
    table_columns = [""] + ["%d Day Change" % x for x in period]
    table_data = []

    # issue label diffs, starting with overall
    if categories[I]:
        # TODO evaluate issue label filter approach
        label_totals = metrics.issue_totals[metrics.OV]
        table_data += [[metrics.OV] + ["+" + str(label_totals[metrics.NEW][-1] - label_totals[metrics.NEW][-x]) +
                                       " / -" + str(label_totals[metrics.DONE][-1] - label_totals[metrics.DONE][-x])
                                       if x <= len(metrics.issue_dates) else "" for x in period]]
        for key in categories[I]:
            if key == metrics.OV:
                continue
            label_totals = metrics.issue_totals[key]
            row = [key] + ["+" + str(label_totals[metrics.NEW][-1] - label_totals[metrics.NEW][-x]) +
                           " / -" + str(label_totals[metrics.DONE][-1] - label_totals[metrics.DONE][-x])
                           if x <= len(metrics.issue_dates) else "" for x in period]
            table_data.append(row)

    # manual sloc diffs
    if categories[S]:
        dates = metrics.sloc_totals[metrics.DATE]
        for key in categories[S]:
            if key == metrics.DATE:
                continue
            label_totals = metrics.sloc_totals.get(key)
            if label_totals is None:
                continue
            row = [key] + [str(label_totals[-1] - label_totals[-x])
                           if x <= len(dates) else "" for x in period]
            for index, value in enumerate(row):
                if index == 0:
                    continue
                if value and int(value) >= 0:
                    row[index] = '+' + value
            table_data.append(row)

    # component counts
    if categories[C]:
        dates = metrics.comp_totals[metrics.DATE]
        for key in categories[C]:
            if key == metrics.DATE:
                continue
            label_totals = metrics.comp_totals.get(key)
            if label_totals is None:
                continue
            row = [key] + [str(label_totals[-1] - label_totals[-x])
                           if x <= len(dates) else "" for x in period]
            for index, value in enumerate(row):
                if index == 0:
                    continue
                if value and int(value) >= 0:
                    row[index] = '+' + value
            table_data.append(row)

    output_file = generate_table(table_columns, table_data, title, directory, show)
    return output_file


def table_issue_label_diffs(reporter, categories=None, period=None, show=False, directory=None, title="Issue Label Changes"):
    categories = {I: categories[I], S: [], C: []}
    return table_project_diffs(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def table_sloc_diffs(reporter, categories=None, period=None, show=False, directory=None, title="Component SLOC Changes"):
    categories = {I: [], S: categories[S], C: []}
    return table_project_diffs(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def table_comp_diffs(reporter, categories=None, period=None, show=False, directory=None, title="Component Structure Changes"):
    categories = {I: [], S: [], C: categories[C]}
    return table_project_diffs(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def table_task_list(reporter, categories=None, period=None, show=False, directory=None, title="Planned Task List"):
    metrics = reporter.metrics

    table_columns = metrics.task_items_header
    table_data = [[task] + [metrics.plan_dict[task][header] for header in metrics.task_items_header[1:]]
                  for task in metrics.plan_task_list]
    #
    # table_data = [metrics.task_items[task] for task in metrics.plan_task_list]

    output_file = generate_table(table_columns, table_data, title, directory, show)
    return output_file


def line_plot_trend(reporter, categories=None, period=None, show=False, directory=None, title=""):
    metrics = reporter.metrics

    period_dates = []
    data_source = []
    data_categories = []

    if categories[I]:
        period_dates = np.array(metrics.issue_dates)
        data_categories = [metrics.NEW, metrics.DONE, metrics.OPEN]
        data_source = metrics.issue_totals[metrics.OV]

    elif categories[S]:
        period_dates = np.array(metrics.sloc_totals[metrics.DATE])
        data_categories = categories[S]
        data_source = metrics.sloc_totals

    elif categories[C]:
        period_dates = np.array(metrics.comp_totals[metrics.DATE])
        data_categories = categories[C]
        data_source = metrics.comp_totals

    output_file = generate_line_plot(period_dates, data_source, data_labels=data_categories,
                                     title=title, directory=directory, show=show, date_plot=True)
    return output_file


def line_plot_issue_labels_trend(reporter, categories=None, period=None, show=False, directory=None, title="Issue Label Trendline"):
    categories = {I: categories[I], S: [], C: []}
    return line_plot_trend(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def line_plot_comp_trend(reporter, categories=None, period=None, show=False, directory=None, title="Component Structure Trendline"):
    categories = {I: [], S: [], C: categories[C]}
    return line_plot_trend(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def line_plot_sloc_trend(reporter, categories=None, period=None, show=False, directory=None, title="Component SLOC Trendline"):
    categories = {I: [], S: categories[S], C: []}
    return line_plot_trend(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def stacked_bar_plot_trend(reporter, categories=None, period=None, show=False, directory=None, title=""):
    metrics = reporter.metrics
    data = {}
    data_categories = []

    # TODO evaluate issue label filter approach
    if categories[I]:
        period = min(len(metrics.issue_dates), max(period))
        period_dates = np.array(metrics.issue_dates[-period:])
        for key in categories[I]:
            if key == metrics.OV:
                continue
            data[key] = np.array(metrics.issue_totals[key][metrics.OPEN][-period:])
            data_categories.append(key)

    # else if component sloc types defined
    elif categories[S]:
        period = min(len(metrics.sloc_totals[metrics.DATE]), max(period))
        period_dates = metrics.sloc_totals[metrics.DATE][-period:]
        for key in sorted(categories[S]):
            data[key] = np.array(metrics.sloc_totals[key][-period:])
            data_categories.append(key)

    # else if component structure types defined
    elif categories[C]:
        period = min(len(metrics.comp_totals[metrics.DATE]), max(period))
        period_dates = metrics.comp_totals[metrics.DATE][-period:]
        for key in sorted(categories[C]):
            data[key] = np.array(metrics.comp_totals[key][-period:])
            data_categories.append(key)

    else:
        raise ValueError("No categories specified for visualization.")

    if period == 1:
        print("Warning: Unable to produce " + title + " with available data points. Visualization will be skipped.")
        return

    output_file = generate_stacked_bar_plot(period_dates, data, data_labels=data_categories,
                                            title=title, directory=directory, show=show, date_plot=True)
    return output_file


def stacked_bar_plot_issue_labels_trend(reporter, categories=None, period=None, show=False, directory=None, title="Issue Label Trend"):
    categories = {I: categories[I], S: [], C: []}
    return stacked_bar_plot_trend(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def stacked_bar_plot_comp_trend(reporter, categories=None, period=None, show=False, directory=None, title="Component Structure Trend"):
    categories = {I: [], S: [], C: categories[C]}
    return stacked_bar_plot_trend(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def stacked_bar_plot_sloc_trend(reporter, categories=None, period=None, show=False, directory=None, title="Component SLOC Trend"):
    categories = {I: [], S: categories[S], C: []}
    return stacked_bar_plot_trend(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def stacked_bar_plot_compare(reporter, categories=None, period=None, show=False, directory=None, title=""):
    metrics = reporter.metrics

    if categories[S]:
        components = sorted(metrics.sloc_data.keys())
        data_categories = categories[S]
        data_source = metrics.sloc_data

    # else if component categories defined
    elif categories[C]:
        components = sorted(metrics.comp_data.keys())
        data_categories = categories[C]
        data_source = metrics.comp_data

    else:
        raise ValueError("No categories specified for visualization.")

    data = {}
    for key in data_categories:
        data[key] = np.array([data_source[component][key] if data_source[component].get(key) is not None else 0
                              for component in components])

    output_file = generate_stacked_bar_plot(components, data, data_labels=data_categories,
                                            title=title, directory=directory, show=show)
    return output_file


def stacked_bar_plot_comp_compare(reporter, categories=None, period=None, show=False, directory=None, title="Component Structure Comparison"):
    categories = {I: [], S: [], C: categories[C]}
    return stacked_bar_plot_compare(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def stacked_bar_plot_sloc_compare(reporter, categories=None, period=None, show=False, directory=None, title="Component SLOC Comparison"):
    categories = {I: [], S: categories[S], C: []}
    return stacked_bar_plot_compare(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def bar_plot_component(reporter, categories=None, period=None, show=False, directory=None, title=""):
    metrics = reporter.metrics

    # if sloc categories defined
    if categories[S]:
        components = sorted(metrics.sloc_data.keys())
        data_categories = categories[S]
        data_source = metrics.sloc_data

    # else if component categories defined
    elif categories[C]:
        components = sorted(metrics.comp_data.keys())
        data_categories = categories[C]
        data_source = metrics.comp_data

    else:
        raise ValueError("No categories specified for visualization.")

    # use data structs to build display data
    data = {}
    for key in data_categories:
        data_list = []
        for component in components:
            value = data_source[component].get(key)
            if value is None:
                value = 0
            data_list.append(value)
        data[key] = np.array(data_list)

    output_file = generate_adjacent_bar_plot(components, data, data_labels=data_categories,
                                             title=title, directory=directory, show=show)
    return output_file


def bar_plot_sloc(reporter, categories=None, period=None, show=False, directory=None, title="Component SLOC"):
    categories = {I: [], S: categories[S], C: []}
    return bar_plot_component(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def bar_plot_comp(reporter, categories=None, period=None, show=False, directory=None, title="Component Structure"):
    categories = {I: [], S: [], C: categories[C]}
    return bar_plot_component(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def split_bar_plot_component(reporter, categories=None, period=None, show=False, directory=None, title=""):
    metrics = reporter.metrics

    # if sloc categories defined
    if categories[S]:
        components = sorted(metrics.sloc_data.keys())
        data_categories = categories[S]
        data_source = metrics.sloc_data

    # else if component categories defined
    elif categories[C]:
        components = sorted(metrics.comp_data.keys())
        data_categories = categories[C]
        data_source = metrics.comp_data

    else:
        raise ValueError("No categories specified for visualization.")

    # use data structs to build display data
    data = {}
    for key in data_categories:
        data_list = []
        for component in components:
            value = data_source[component].get(key)
            if value is None:
                value = 0
            data_list.append(value)
        data[key] = np.array(data_list)

    output_file = generate_split_bar_plot(components, data, data_labels=data_categories,
                                          title=title, directory=directory, show=show)
    return output_file


def split_bar_plot_sloc(reporter, categories=None, period=None, show=False, directory=None, title="Component SLOC"):
    categories = {I: [], S: categories[S], C: []}
    return split_bar_plot_component(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def split_bar_plot_comp(reporter, categories=None, period=None, show=False, directory=None, title="Component Structure"):
    categories = {I: [], S: [], C: categories[C]}
    return split_bar_plot_component(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def pie_plot_issue_labels_totals(reporter, categories=None, period=None, show=False, directory=None, title="Issue Label Totals"):
    metrics = reporter.metrics
    keys_list = []
    totals_list = []

    # TODO evaluate issue label filter approach

    for key in sorted(metrics.issue_totals.keys()):
        if key == metrics.OV:
            continue
        amount = metrics.issue_totals[key][metrics.NEW][-1] - metrics.issue_totals[key][metrics.DONE][-1]
        if amount > 0:
            keys_list.append(key)
            totals_list.append(amount)
    overall_amount = metrics.issue_totals[metrics.OV][metrics.NEW][-1] - metrics.issue_totals[metrics.OV][metrics.DONE][-1]
    if (sum(totals_list)) < overall_amount:
        keys_list.append(metrics.UL)
        totals_list.append(overall_amount - sum(totals_list))
    total = sum(totals_list)

    fig, ax = plt.subplots(1, 1, figsize=(10, 10))
    if len(keys_list) > 10:
        cmap = plt.get_cmap(SECONDARY_CMAP)
        colors = cmap(np.linspace(0., 1., len(keys_list)))
    else:
        cmap = plt.get_cmap(DEFAULT_CMAP)
        colors = cmap(np.arange(len(keys_list)))
    ax.pie(totals_list, labels=keys_list, colors=colors, autopct=lambda p: '{0:.0f} ({1:.2f}%)'.format(p * total / 100, p))
    # plt.title(title)
    ax.set_title(title)
    fig.tight_layout()

    if show:
        plt.show()
        plt.close(fig)
        return
    if directory is not None:
        output = directory + title + ".png"
        output = output.replace(" ", "_")
        plt.savefig(output)
        plt.close(fig)
        return output


def pie_plot_category_totals(reporter, categories=None, period=None, show=False, directory=None, title=""):
    metrics = reporter.metrics

    # data struct prep
    components = []
    data_categories = []
    data_source = {}

    # if sloc categories defined
    if categories[S]:
        components = sorted(metrics.sloc_data.keys())
        data_categories = categories[S]
        data_source = metrics.sloc_data

    # else if component categories defined
    elif categories[C]:
        components = sorted(metrics.comp_data.keys())
        data_categories = categories[C]
        data_source = metrics.comp_data

    data_values = []
    data_labels = []
    for component in components:
        data_values.append(0)
        data_labels.append(component)
        for category in data_categories:
            value = data_source[component].get(category)
            if value is None:
                value = 0
            data_values[-1] += value
    total = sum(data_values)

    # for index, value in enumerate(data_values):
    #     if value == 0 and not reporter.config_opts.metrics_display_zero_comps:
    #         del[data_values[index]]
    #         del[data_labels[index]]

    fig, ax = plt.subplots(1, 1, figsize=(10, 10))
    if len(components) > 10:
        cmap = plt.get_cmap(SECONDARY_CMAP)
        colors = cmap(np.linspace(0., 1., len(components)))
    else:
        cmap = plt.get_cmap(DEFAULT_CMAP)
        colors = cmap(np.arange(len(components)))
    ax.pie(data_values, labels=data_labels, colors=colors, startangle=90,
           autopct=lambda p: '{0:.0f} ({1:.2f}%)'.format(p * total / 100, p))
    ax.set_title(title)
    fig.tight_layout()

    if show:
        plt.show()
        plt.close(fig)
        return
    if directory is not None:
        output = directory + title + ".png"
        output = output.replace(" ", "_")
        plt.savefig(output)
        plt.close(fig)
        return output


def pie_plot_sloc_totals(reporter, categories=None, period=None, show=False, directory=None, title="Component SLOC Totals"):
    categories = {I: [], S: categories[S], C: []}
    return pie_plot_category_totals(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def pie_plot_comp_totals(reporter, categories=None, period=None, show=False, directory=None, title="Component Structure Totals"):
    categories = {I: [], S: [], C: categories[C]}
    return pie_plot_category_totals(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def pie_plot_category_overview(reporter, categories=None, period=None, show=False, directory=None, title=""):
    metrics = reporter.metrics

    # data struct prep
    components = []
    data_categories = []
    data_source = {}

    # if sloc categories defined
    if categories[S]:
        components = sorted(metrics.sloc_data.keys())
        data_categories = categories[S]
        data_source = metrics.sloc_data

    # else if component categories defined
    elif categories[C]:
        components = sorted(metrics.comp_data.keys())
        data_categories = categories[C]
        data_source = metrics.comp_data

    data = []
    data_labels = []
    for category in data_categories:
        data.append(0)
        data_labels.append(category)
        for component in components:
            value = data_source[component].get(category)
            if value is None:
                value = 0
            data[-1] += value
    total = sum(data)

    if len(data_categories) > 10:
        cmap = plt.get_cmap(SECONDARY_CMAP)
        colors = cmap(np.linspace(0., 1., len(data_categories)))
    else:
        cmap = plt.get_cmap(DEFAULT_CMAP)
        colors = cmap(np.arange(len(data_categories)))
    fig, ax = plt.subplots(1, 1, figsize=(10, 10))
    ax.pie(data, labels=data_labels, colors=colors, autopct=lambda p: '{0:.0f} ({1:.2f}%)'.format(p * total / 100, p))
    # plt.title(title)

    ax.set_title(title)
    fig.tight_layout()

    if show:
        plt.show()
        plt.close(fig)
        return
    if directory is not None:
        output = directory + title + ".png"
        output = output.replace(" ", "_")
        plt.savefig(output)
        plt.close(fig)
        return output


def pie_plot_sloc_overview(reporter, categories=None, period=None, show=False, directory=None, title="Component SLOC Overview"):
    categories = {I: [], S: categories[S], C: []}
    return pie_plot_category_overview(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def pie_plot_comp_overview(reporter, categories=None, period=None, show=False, directory=None, title="Component Structure Overview"):
    categories = {I: [], S: [], C: categories[C]}
    return pie_plot_category_overview(reporter, categories=categories, period=period, show=show, directory=directory, title=title)


def bar_plot_sloc_snapshot(reporter, categories=None, period=None, show=False, directory=None, title="Component SLOC Snapshot"):
    metrics = reporter.metrics
    config = reporter.config_opts

    data_source = []
    data_categories = []

    if categories[S]:
        for category in categories[S]:
            data_categories.append(category)
            data_source.append(metrics.sloc_totals[category][-1])
        if config.metrics_sloc_estimation is not None:
            data_categories.append("Estimated")
            data_source.append(int(config.metrics_sloc_estimation))

    else:
        return

    cmap = plt.get_cmap(DEFAULT_CMAP)
    colors = cmap(np.arange(len(data_categories)))

    fig, ax = plt.subplots(1, 1, figsize=(10, 10))
    xax = np.arange(len(data_categories))
    bars = ax.bar(xax, np.array(data_source))

    index = 0
    for bar in bars:
        bar.set_color(colors[index])
        index += 1
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width() / 2., 1.05 * height, '%d' % int(height), ha='center', va='bottom')

    plt.grid()

    ax.set_xticklabels(data_categories, rotation=60)
    ax.set_title(title)
    fig.tight_layout()

    if show:
        plt.show()
        plt.close(fig)
        return
    if directory is not None:
        output = directory + title + ".png"
        output = output.replace(" ", "_")
        plt.savefig(output)
        plt.close(fig)
        return output


def bar_chart_active_tasks(reporter, categories=None, period=None, show=False, directory=None, title="Active Task Progress"):
    metrics = reporter.metrics
    today = datetime.date.today()

    xc = "Expected completion"
    cc = "Current completion"

    active_tasks = []
    task_names = []
    task_progress = {xc: [], cc: []}
    categories = [xc, cc]

    for task_key in metrics.plan_task_list:
        if metrics.plan_dict[task_key][metrics.pv.START] <= today or metrics.plan_dict[task_key][metrics.CV] > 0:
            active_tasks.append(metrics.plan_dict[task_key])

    active_tasks = sorted(active_tasks, key=lambda task_item: task_item[metrics.pv.START])
    for task in active_tasks:
        pv = task[metrics.pv.EV]
        if task[metrics.XV] < pv or task[metrics.CV] < pv:
            task_names.append(task[metrics.pv.TASK])
            task_progress[xc].append(round(task[metrics.XV] / pv, 2))
            task_progress[cc].append(round(task[metrics.CV] / pv, 2))

    output_file = generate_adjacent_bar_plot(task_names, task_progress, data_labels=categories,
                                             title=title, directory=directory, show=show)
    return output_file


def line_plot_ev_plan(reporter, categories=None, period=None, show=False, directory=None, title="Task Progress vs Plan"):
    metrics = reporter.metrics

    data = {}
    data_x = {}
    data_categories = []
    filled = [metrics.EV]

    if metrics.plan_progress:
        data[metrics.EV] = metrics.plan_progress[metrics.EV]
        data_x[metrics.EV] = metrics.plan_progress[metrics.DATE]
        data_categories += [metrics.EV]

    for plan_key in sorted(metrics.plan_totals.keys()):
        data[plan_key] = metrics.plan_totals[plan_key][metrics.EV]
        data_x[plan_key] = metrics.plan_totals[plan_key][metrics.DATE]
        data_categories += [plan_key]

    output_file = generate_line_plot(data_x, data, filled=filled, data_labels=data_categories,
                                     title=title, directory=directory, show=show, date_plot=True)
    return output_file


def sloc_overview_annotation(reporter, categories=None, period=None, show=False, directory=None, title="SLOC Overview Annotation"):
    return annotation_insert(list(reporter.metrics.sloc_data.keys()))


def sloc_totals_annotation(reporter, categories=None, period=None, show=False, directory=None, title="SLOC Totals Annotation"):
    types = [reporter.metrics.AC + t for t in categories[S]] + [reporter.metrics.MC + t for t in categories[S]]
    return annotation_insert(types)


def comp_overview_annotation(reporter, categories=None, period=None, show=False, directory=None, title="Component Overview Annotation"):
    return annotation_insert(list(reporter.metrics.comp_data.keys()))


def comp_totals_annotation(reporter, categories=None, period=None, show=False, directory=None, title="Component Totals Annotation"):
    types = categories[C]
    return annotation_insert(types)


def annotation_insert(types):
    component_string = "    "
    for component in sorted(types):
        component_string += component + ", "
        if len(component_string) > 120:
            newline = component_string[:-2].rfind(', ')
            if newline > 0:
                component_string = component_string[:newline + 2] + "\n    " + component_string[newline + 2:]
    component_string = "Included types:\n" + component_string[:-2] + "\n\n"
    return component_string


def _sort_legend(ax):
    handles, labels = ax.get_legend_handles_labels()
    # sort both labels and handles by labels
    labels, handles = list(zip(*sorted(zip(labels, handles), key=lambda t: t[0])))
    return handles, labels


def _reverse_legend(ax):
    handles, labels = ax.get_legend_handles_labels()
    # reverse both labels and handles by labels
    labels = labels[::-1]
    handles = handles[::-1]
    return handles, labels


def main(args):
    ghereporter = GitHubMetricsReport(args)
    # ghereporter.metrics.export()

    metrics = ghereporter.metrics
    config_opts =ghereporter.config_opts

    categories = {}
    if categories.get(I) is None:
        categories[I] = sorted(metrics.issue_totals.keys()) \
                        if '*' in config_opts.metrics_issue_labels else config_opts.metrics_issue_labels
        # categories[I] = sorted(list(set(metrics.issue_totals.keys()) - set(config_opts.pipeline_weights.keys()))) \
        #                 if '*' in config_opts.metrics_issue_labels else config_opts.metrics_issue_labels
    if categories.get(I) is None:
        categories[I] = DEFAULT_ISSUE_LABELS

    if categories.get(S) is None:
        categories[S] = config_opts.metrics_sloc_types
    if categories.get(S) is None:
        categories[S] = DEFAULT_SLOC_TYPES
    if categories[S] == ["sloc"]:
        categories[S] = [val + "sloc" for val in [metrics.MC, metrics.AC, metrics.XML]]

    if categories.get(C) is None:
        categories[C] = config_opts.metrics_comp_types
    if categories.get(C) is None:
        categories[C] = DEFAULT_COMP_TYPES

    period = config_opts.metrics_periods or DEFAULT_TREND_RANGE

    show = config_opts.metrics_report_filename is None or ghereporter.show
    report_dir = config_opts.metrics_report_dir
    artifact_dir = config_opts.metrics_report_artifact_dir

    # verify / prepare output structure
    if report_dir != "":
        if report_dir[-1] != "/":
            report_dir = report_dir + "/"
        # attempt to create directory, an errno of 17 means it already exists and we can ignore it
        try:
            os.mkdir(report_dir)
        except OSError as err:
            if err.errno != 17:
                print("ERROR: " + err.message)

    if artifact_dir != "":
        if artifact_dir[-1] != "/":
            artifact_dir = artifact_dir + "/"
        if artifact_dir.startswith(report_dir):
            artifact_dir = artifact_dir[len(report_dir):]
        # attempt to create directory, an errno of 17 means it already exists and we can ignore it
        try:
            os.mkdir(report_dir + artifact_dir)
        except OSError as err:
            if err.errno != 17:
                print("ERROR: " + err.message)

    if config_opts.force_remote_history:
        try:
            os.mkdir(report_dir + "history/")
        except OSError as err:
            if err.errno != 17:
                print("ERROR: " + err.message)

    directory = report_dir + artifact_dir

    # generate report artifacts and visualizations
    artifacts = []

    for section in config_opts.metrics_report_sections:
        try:
            artifacts.append(globals()[section](ghereporter, categories=categories, period=period, directory=directory, show=show))
            # getattr(githubmetricsreport, 'bar')()
        except BaseException as err:
            print("Visualization " + section + " had an error and can not be generated. Error follows:")
            print(err.message)

    # build the artifacts into a markdown report
    if artifacts:
        report_file = config_opts.metrics_report_filename
        with file(report_dir + report_file, "wb") as fp:
            fp.write("Report generated: {}\n\n".format(str(datetime.date.today())))
            for artifact in artifacts:
                if artifact is None:
                    continue
                if ".png" in artifact:
                    artifact_file = artifact[len(report_dir):]
                    section_name = artifact[len(directory):-4].replace("_", " ")
                    line = "## " + section_name + "\n![" + section_name + "](./" + artifact_file + ")\n\n"
                else:
                    line = artifact
                fp.write(line)
    return


def output_mapping(location=""):
    fs = []
    maxi = 0
    with open(location + "metricsreport.py") as file:
        for line in file:
            if "def" in line and "title=" in line:
                func = line[4:line.find('(')]
                title = line[line.find("title=\"") + 7:line.rfind('"')]
                if len(title) > maxi:
                    maxi = len(title)
                if title == "" or "Annotation" in title:
                    continue
                fs.append((title, func))

    fs = sorted(fs)
    for f, t in fs:
        print('| ' + f + " | " + t + " |  |")
    return


if __name__ == '__main__':
    main(sys.argv)
