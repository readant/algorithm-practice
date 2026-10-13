import sys, random, subprocess, itertools
from collections import deque

BUILD = "E:/Projects0/algorithm-practice/题解归档/牛客网/牛客周赛 Round 165/_verify/"

def run(exe, inp):
    return subprocess.run([BUILD + exe], input=inp, capture_output=True, text=True).stdout

# ---------- C ----------
def brute_C(n, a):
    cnt = 0
    for k in range(1, n + 1):
        b = a[:k][::-1] + a[k:]
        if all(b[i] <= b[i + 1] for i in range(n - 1)):
            cnt += 1
    return cnt

def check_C(trials=300):
    random.seed(7); cases = []
    for _ in range(trials):
        n = random.randint(1, 9)
        cases.append((n, [random.randint(-4, 4) for _ in range(n)]))
    inp = [str(len(cases))]
    for n, a in cases:
        inp += [str(n), " ".join(map(str, a))]
    out = run("c_sol.exe", "\n".join(inp) + "\n").split()
    bad = 0
    for (n, a), got in zip(cases, out):
        if int(got) != brute_C(n, a):
            bad += 1
            if bad <= 5: print(f"C fail n={n} a={a} got={got} exp={brute_C(n, a)}")
    print(f"[C] {trials-bad}/{trials} OK")

# ---------- D ----------
def brute_D(n, a):
    best = None
    for c in set(a):
        b = [x for x in a if x != c]
        s = sum(x if i % 2 == 0 else -x for i, x in enumerate(b))
        best = abs(s) if best is None else min(best, abs(s))
    return best if best is not None else 0

def check_D(trials=300):
    random.seed(11); bad = 0
    for _ in range(trials):
        n = random.randint(1, 10)
        a = [random.randint(-5, 5) for _ in range(n)]
        got = run("d_sol.exe", f"{n}\n" + " ".join(map(str, a)) + "\n").strip()
        if int(got) != brute_D(n, a):
            bad += 1
            if bad <= 5: print(f"D fail n={n} a={a} got={got} exp={brute_D(n, a)}")
    print(f"[D] {trials-bad}/{trials} OK")

# ---------- E ----------
def brute_E(n, colors, edges):
    adj = [[] for _ in range(n + 1)]
    for u, v in edges:
        adj[u].append(v); adj[v].append(u)
    cnt = 0
    for u in range(1, n + 1):
        for v in range(u + 1, n + 1):
            if colors[u - 1] != colors[v - 1]:
                continue
            par = [0] * (n + 1); seen = [False] * (n + 1); seen[u] = True
            dq = deque([u])
            while dq:
                x = dq.popleft()
                for y in adj[x]:
                    if not seen[y]:
                        seen[y] = True; par[y] = x; dq.append(y)
            path = []; x = v
            while x != u:
                path.append(x); x = par[x]
            path.append(u); path.reverse()
            ch = sum(1 for i in range(len(path) - 1) if colors[path[i] - 1] != colors[path[i + 1] - 1])
            if ch == 2: cnt += 1
    return cnt

def check_E(trials=300):
    random.seed(13); bad = 0
    for _ in range(trials):
        n = random.randint(1, 9)
        colors = [random.randint(1, 3) for _ in range(n)]
        edges = [(random.randint(1, i - 1), i) for i in range(2, n + 1)]
        inp = f"{n}\n" + " ".join(map(str, colors)) + "\n"
        inp += "".join(f"{u} {v}\n" for u, v in edges)
        got = run("e_sol.exe", inp).strip()
        exp = brute_E(n, colors, edges)
        if int(got) != exp:
            bad += 1
            if bad <= 5: print(f"E fail n={n} col={colors} ed={edges} got={got} exp={exp}")
    print(f"[E] {trials-bad}/{trials} OK")

# ---------- F ----------
def brute_F(n, ranges):
    best = None
    for xs in itertools.product(*[range(l, r + 1) for l, r in ranges]):
        ok = False
        for p in range(1, n - 1):
            if all(xs[i] < xs[i + 1] for i in range(p)) and all(xs[i] > xs[i + 1] for i in range(p, n - 1)):
                ok = True; break
        if ok:
            m = max(abs(xs[i + 1] - xs[i]) for i in range(n - 1))
            best = m if best is None else min(best, m)
    return best if best is not None else -1

def check_F(trials=200):
    random.seed(17); bad = 0
    for _ in range(trials):
        n = random.randint(3, 6)
        ranges = []
        for _ in range(n):
            l = random.randint(-3, 2); r = l + random.randint(0, 2)
            ranges.append((l, r))
        inp = f"{n}\n" + "".join(f"{l} {r}\n" for l, r in ranges)
        got = run("f_sol.exe", inp).strip()
        exp = brute_F(n, ranges)
        if int(got) != exp:
            bad += 1
            if bad <= 5: print(f"F fail n={n} ranges={ranges} got={got} exp={exp}")
    print(f"[F] {trials-bad}/{trials} OK")

if __name__ == "__main__":
    check_C(); check_D(); check_E(); check_F()
