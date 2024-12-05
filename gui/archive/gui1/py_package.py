import os
if __name__ == '__main__':
    from PyInstaller.__main__ import run
    opts=['gui.py','-w','-F','--icon=favicon.ico','--add-data=gui.ui;.','--add-data=favicon.ico;.']
    run(opts)

