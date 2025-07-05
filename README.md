# `walltaker.sh`
A basic wallpaper setting cli for [Walltaker](https://walltaker.joi.how/)
> This program takes inspiration from [Plush Pawrest's (Joseph Berliner)](https://gitlab.com/JBerliner) [walltaker-client](https://gitlab.com/JBerliner/walltaker-client)
## Why I made this
I use the i3 window manager so to use walltaker i downloaded [Plush Pawrest's](https://gitlab.com/JBerliner) [walltaker-client](https://gitlab.com/JBerliner/walltaker-client). When reviewing the programs code I saw a few main things that I wanted different, one of those being the way it handles the actual setting of the wallpaper as on my desktop I use a custom script to actually set the wallpaper.

## Requirements
- sed 
- curl 
- echo 
- mv 
- notify-send (optional depending on settings)
- bash
## List of Improvements / Features

| type | title |
|-|-|
| Improvment | Basic argument parsing.|
| Feature | Added user-client option | 
| Improvement | Moved settings into the file so no configuration file is required | 
| Feature | Added a wallpaper description var for notifications | 
| Feature | Added self kill method, if it was to run for a second time it will kill all instances of itself so that only one script is running. | 
| Feature | uses eval to set the wallpaper using a command, this is useful as walltaker.sh can run any script with any arguments when it changes the wallpaper | 
# Usage 
```
walltaker.sh 
--id         <string>
--path       <string>
--delay      <int>
--user-agent <string>

--no-i3
--no-notify
```
# Installation 
> Seeing as this is just a script the installation is portable
```
git clone https://github.com/9JH1/walltaker.sh
cd walltaker.sh
bash walltaker.sh
```
open walltaker.sh to change the wallpaper_set_command var, this is run when the wallpaper has been updated where `$location` is the new wallpaper path.

# Updates
> I intend on rewriting this in C soon so stay updated.. 
```
cd walltaker.sh 
git pull
```