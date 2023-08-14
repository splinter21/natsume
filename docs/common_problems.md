# Common Questions


### ImportError: incompatible architecture

Successfully installed Natsume using `pip install natsume` but got an error when importing.

```bash
Traceback (most recent call last):
  File "/Users/username/Desktop/openjtalk/playground/tempCodeRunnerFile.py", line 1, in <module>
    from natsume import Natsume
  File "/Users/username/anaconda3/lib/python3.10/site-packages/natsume/__init__.py", line 1, in <module>
    from .frontend import Natsume
  File "/Users/username/anaconda3/lib/python3.10/site-packages/natsume/frontend.py", line 1, in <module>
    from natsume.oj import OpenjtalkFrontend
  File "/Users/hurui05/anaconda3/lib/python3.10/site-packages/natsume/oj.py", line 1, in <module>
    from natsume.openjtalk import OpenJTalk
ImportError: dlopen(/Users/username/anaconda3/lib/python3.10/site-packages/natsume/openjtalk.cpython-310-darwin.so, 0x0002): tried: '/Users/username/anaconda3/lib/python3.10/site-packages/natsume/openjtalk.cpython-310-darwin.so' (mach-o file, but is an incompatible architecture (have 'x86_64', need 'arm64')), '/System/Volumes/Preboot/Cryptexes/OS/Users/username/anaconda3/lib/python3.10/site-packages/natsume/openjtalk.cpython-310-darwin.so' (no such file), '/Users/username/anaconda3/lib/python3.10/site-packages/natsume/openjtalk.cpython-310-darwin.so' (mach-o file, but is an incompatible architecture (have 'x86_64', need 'arm64'))
```

If your are using Mac OSX, try the following commands.

```bash
pip uninstall natsume

ARCHFLAGS="-arch arm64" pip install natsume --no-cache-dir
```
