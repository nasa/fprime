import random

MEMBER_SIZES = 3

with open("fp_profiling", "r") as fh:
    lines = [line.strip().split() for line in fh.readlines()]

totals = {}
lasts = {}
for stamp, stage, token, module in lines:
    stamp = float(stamp)
    if token not in totals:
        totals[token] = (0, 0, 9999999999, 0, [])
    if stage == "START":
        lasts[token] = stamp
    elif stage == "STOP":
        accum, count, min_delta, max_delta, hits = totals[token]
        delta = stamp - lasts[token]
        totals[token] = (
            accum + delta,
            count + 1,
            min(delta, min_delta),
            max(delta, max_delta),
            hits + [(delta, module)],
        )

for token, pair in totals.items():
    accum, count, min_delta, max_delta, hits = totals[token]
    if count == 0:
        continue
    print(
        "{} {:.6f} ({:.6f}, {:.6f}, {:.6f}) {}".format(
            token, accum, min_delta, accum / count, max_delta, hits[int(count / 2)][0]
        )
    )
    hist = {index: (0, []) for index in range(0, 1000)}
    for item, module in hits:
        index = int(int(item * 1000) / 10) * 10
        if index not in hist:
            hist[index] = (0, [])
        hist[index] = (hist[index][0] + 1, hist[index][1] + [module])
    for index, pair in hist.items():
        count, members = pair
        random.shuffle(members)
        if count != 0:
            print(
                "    {:3} ms: {:4}        -- {}".format(
                    index,
                    count,
                    " ".join(members[:MEMBER_SIZES])
                    + ("" if len(members) <= MEMBER_SIZES else " ..."),
                )
            )
