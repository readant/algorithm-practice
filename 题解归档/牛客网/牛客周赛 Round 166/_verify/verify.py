# -*- coding: utf-8 -*-
"""C / D 两题的正解对拍验证。

用法（在仓库根或本目录下）：
    python _verify/verify.py

依赖：本机 g++，会临时把正解编译到系统临时目录。
"""
import os
import random
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)


def build(src, out):
    subprocess.run(
        ["g++", "-std=c++11", "-O2", "-o", out, src],
        check=True,
    )


def run(exe, inp):
    p = subprocess.run([exe], input=inp.encode(), capture_output=True)
    if p.returncode != 0:
        raise RuntimeError(p.stderr.decode("utf-8", "ignore"))
    return p.stdout.decode().strip()


def brute_c(n, p, lrs):
    cnt = 0
    for i in range(n):
        li, ri = lrs[i]
        dl = 0
        dr = 0
        for j in range(n):
            lj, rj = lrs[j]
            if lj <= li <= rj:
                dl += 1
            if lj <= ri <= rj:
                dr += 1
        if abs(dl - dr) >= p:
            cnt += 1
    return cnt


def brute_d(a):
    n = len(a)
    cnt = 0
    for l in range(n):
        for r in range(l + 2, n):
            s = 0
            for t in range(l + 1, r):
                s += a[t]
            if s == a[l] * a[r]:
                cnt += 1
    return cnt


def check_c(exe, rounds=300):
    random.seed(16601)
    for t in range(rounds):
        n = random.randint(1, 120)
        p = random.randint(0, n)
        lrs = []
        for _ in range(n):
            l = random.randint(1, 30)
            r = random.randint(l, 30)
            lrs.append((l, r))
        inp = "%d %d\n" % (n, p) + "".join("%d %d\n" % x for x in lrs)
        got = run(exe, inp)
        exp = str(brute_c(n, p, lrs))
        if got != exp:
            print("[C] FAIL round=%d n=%d p=%d\ninput:\n%s\ngot=%s exp=%s" % (t, n, p, inp, got, exp))
            return False
    print("[C] %d 组全部一致" % rounds)
    return True


def check_d(exe, rounds=300):
    random.seed(16602)
    for t in range(rounds):
        n = random.randint(3, 60)
        a = [random.randint(1, 6) for _ in range(n)]
        inp = "%d\n%s\n" % (n, " ".join(map(str, a)))
        got = run(exe, inp)
        exp = str(brute_d(a))
        if got != exp:
            print("[D] FAIL round=%d n=%d\ninput:\n%s\ngot=%s exp=%s" % (t, n, inp, got, exp))
            return False
    print("[D] %d 组全部一致" % rounds)
    return True


def main():
    tmp = tempfile.mkdtemp(prefix="nc166v_")
    c_src = os.path.join(ROOT, "C_小月的长廊", "C_小月的长廊.cpp")
    d_src = os.path.join(ROOT, "D_小月的回声", "D_小月的回声.cpp")
    c_exe = os.path.join(tmp, "C.exe")
    d_exe = os.path.join(tmp, "D.exe")
    build(c_src, c_exe)
    build(d_src, d_exe)
    ok = check_c(c_exe) and check_d(d_exe)
    print("ALL PASS" if ok else "FAILED")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
