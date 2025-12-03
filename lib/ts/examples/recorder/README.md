# Example - Market Data Recorder
This example implemented a market data recorder that can record market data of all future contracts. Starting, stopping, archiving are scheduled by `systemd`.

## How to use
To install the program files, run the following commands in this directory: 

`sudo ./install.sh`

And then, enable the timer:

`sudo systemctl enable --now record.timer`

After doing these, the program will run automatically when trading time is coming, and will be stopped after trading time is ended. 

If you want you remove the files installed, run:

`sudo ./remove.sh`

## Notice
### Directories
- Log file: `/var/lib/webctp/logs/record.log`
- Temporary data directory (.csv files): `/var/lib/webctp/record/YYYYMMDD`
- Archived data directory (.tar.gz files): `/var/lib/webctp/archived` 
- Service unit files: `/etc/systemd/system`
- Typescript directory: `/usr/local/lib/webctp/recorder`

### Schedule:
- 08:40 Start
- 11:40 Stop 
- 13:20 Start
- 15:10 Stop
- 15:30 Archive
- 20:50 Start
- 02:40 Stop