#!/usr/bin/env python

# Python 3 script to quickly build the analyser, re-open Logic and start a simulation
import os
import time
import saleae
import psutil
from build_analyzer import build

LOGIC_PATH = "/home/batman/Desktop/Logic/"

def close_logic():
  for p in psutil.process_iter():
    if p.name() == 'Main':
      p.kill()

def clear_crash_log():
  if os.path.exists('core'):
    os.remove('core')
  
  if os.path.exists(LOGIC_PATH + 'core'):
    os.remove(LOGIC_PATH + 'core')

def test():
  close_logic()
  clear_crash_log()
  build()
  saleae.Saleae.launch_logic(logic_path=LOGIC_PATH + 'Logic', quiet=False)
  time.sleep(1)
  s = saleae.Saleae()
  s.capture_start()

if __name__ == "__main__":
  test()
