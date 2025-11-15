export module threadutil;

import std;
import threadlocalstorage;

export namespace ThreadUtil
{
  thread_local ThreadLocalStorage TLSObj{};
}