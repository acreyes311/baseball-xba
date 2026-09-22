"""Pull league-wide Statcast batted-ball data for a date range.

Dev dataset: June 2026. Swap START_DATE/END_DATE (and OUTPUT_PATH) to a full
season later once the C++ pipeline is validated against this smaller sample.
"""

from pathlib import Path

import pandas as pd
from pybaseball import statcast

START_DATE = "2026-06-01"
END_DATE = "2026-06-30"
OUTPUT_PATH = Path(__file__).resolve().parent.parent / "data" / "statcast_june2026.csv"

# Only the columns the xBA bucket model (and pitcher lookup) actually need,
# out of pybaseball's 119-column pitch-tracking schema. player_name is the
# pitcher (this is pitch-level data), so pitcher is its matching id column.
COLUMNS = [
    "game_date",
    "player_name",
    "pitcher",
    "launch_speed",
    "launch_angle",
    "events",
    "estimated_ba_using_speedangle",
]


def fetch_batted_balls(start_date: str, end_date: str) -> pd.DataFrame:
    pitches = statcast(start_dt=start_date, end_dt=end_date)
    # statcast() returns one row per pitch; only pitches put in play have a
    # recorded launch_speed/launch_angle, which is what the bucket model needs.
    # `events` also has to be non-null: a foul ball is batted (so it has a
    # launch_speed/launch_angle) but doesn't end the plate appearance, so it
    # has no event and isn't a real hit-or-out outcome to bucket on.
    batted_balls = pitches.dropna(subset=["launch_speed", "launch_angle", "events"])
    return batted_balls[COLUMNS]


def main() -> None:
    batted_balls = fetch_batted_balls(START_DATE, END_DATE)
    OUTPUT_PATH.parent.mkdir(exist_ok=True)
    batted_balls.to_csv(OUTPUT_PATH, index=False)
    print(
        f"Wrote {len(batted_balls)} batted-ball rows "
        f"({START_DATE} to {END_DATE}) to {OUTPUT_PATH}"
    )


if __name__ == "__main__":
    main()
