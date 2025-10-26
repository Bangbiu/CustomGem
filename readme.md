# To Create New Component Gem  
## Create Custom Gem
```bash
$ scripts\o3de.bat create-gem -gp D:\Projects\CustomGem
```
## Register Gem in Project
```bash
$ scripts\o3de.bat register -gp D:\Projects\CustomGem -espp D:\Projects\GemDebug
```
## Compile Project with Gem
```bash
$ cmake --build build/windows --target CustomGem.Editor CustomGem --config profile -- -m
```
## Create Component In Gem Folder
```bash
$ scripts\o3de.bat create-from-template -dp D:\Projects\CustomGem\Code -dn CustomGem -tn DefaultComponent -kr -r ${GemName} CustomGem -f
```