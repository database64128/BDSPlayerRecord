# Bedrock Dedicated Server Player Record

A simple tool for parsing Bedrock Dedicated Server (BDS) logs and generate records of players.

## Build

At least `C++ 17` is required.

## Usage

### Arguments

`--append-logdb`: Append to LogDB from the logs directory or a log file.

`--clear-logdb`: Flush the LogDB.

`--build-playerdb`: Build the PlayerDB from the LogDB. You should append LogDB first.

`--csv-logdb`: Export LogDB as a CSV file.

`--csv-playerdb $sortBy`: Export PlayerDB as a CSV file.

#### `$sortBy`

* `0`: No sorting
* `1`: sort by gamertag
* `2`: sort by time played
* `3`: sort by play count
* `4`: sort by last online

*Other undocumented arguments are not intended for use.*

### Example

1. Build the LogDB from `test_log.txt`, then build the PlayerDB. Export all data to CSV files. Player records are sorted by time played.

```
$ ./BDSPlayerRecord --clear-logdb
$ ./BDSPlayerRecord --append-logdb test_log.txt
$ ./BDSPlayerRecord --build-playerdb
$ ./BDSPlayerRecord --csv-logdb
$ ./BDSPlayerRecord --csv-playerdb 2
```

## License

Released under the [GNU GPLv3](LICENSE).