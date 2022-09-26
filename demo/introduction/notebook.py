import matplotlib.pyplot as plt
import math
from IPython.display import IFrame
import subprocess 
import os
import nbformat
import click
from collections import namedtuple

#from IPython.core.display import HTML
#styles = open("./styles/custom.css", "r").read()
#display(HTML('<style>' + styles + '</style>'))
#def css_styling():
#css_styling()

columns=["ET", "IC", "CPI", "MHz", "CT"] # This is the subset of the data we will pay attention to.

#os.environ['CSE142L_LAB'] = 'intro'

# this is crippled versions of the code from later labs.  I hacked it in here for teh programming assigment in lab 1
RenderedCode =namedtuple("RenderedCode", "source asm cfg cfg_counts gprof call_graph")
def shell_cmd(cmd, shell=False, quiet_on_success=False):
    try:
        if not quiet_on_success:
            print(cmd)
        if shell:
            p = subprocess.run(cmd, shell=True, check=True, stderr=subprocess.STDOUT, stdout=subprocess.PIPE)
        else:
            p = subprocess.run(cmd.split(), check=True, stderr=subprocess.STDOUT, stdout=subprocess.PIPE)

        if not quiet_on_success:
            print(p.stdout.decode())
    except subprocess.CalledProcessError as e:
        print(e.output.decode())

    return
    

def build_reps(src, asm, obj, function, gmon=None, run=True, *argc, **kwargs):
    gprof = None
    call_graph = None
    
    cfg_counts = None
    cfg = None
        
    return RenderedCode(source=do_render_code(file=src, lang="c++", show=function),
                        asm=do_render_code(file=asm, lang="gas", show=function),
                        cfg_counts=cfg_counts,
                        cfg=cfg,
                        gprof = gprof,
                        call_graph = call_graph)

user = None

def login(username):
    global user
    user = username
    return  IFrame(f"https://cse142l-dev.wl.r.appspot.com/user/web-login?email={username}", width=500, height=400)
    #return IFrame(f"{os.environ['DJR_SERVER']}/user/web-login?email={username}", width=500, height=400)
    
def token(token):
    cmd = f"cse142 login {user} --token {token}"
    try:
        print(subprocess.check_output(cmd, shell=True, stderr=subprocess.STDOUT).decode())
    except subprocess.CalledProcessError as e:
        print(f"""Authentication failed.  Try the following:
  1.  Click "Sign in" again to get a new token.
  2.  Make sure you are using your "@ucsd.edu" email address.
  3.  Make sure you are completely replacing the old token in the command above.

Here's the output of the authentication command:

{e.output}

If you get something like this:

File "/opt/conda/lib/python3.7/site-packages/pkg_resources/__init__.py", line 792, in resolve
    raise VersionConflict(dist, req).with_context(dependent_req)
pkg_resources.ContextualVersionConflict: (grpcio 1.32.0 (/home/oweng/.local/lib/python3.7/site-packages), Requirement.parse('grpcio<2.0dev,>=1.38.1'), 'google-cloud-pubsub')

that mentions a "ContextualVersionConflict" and includes a path in your home direcotry (e.g., "/home/oweng/.local..."), then you have some python libraries installed in your home directory (probably from an earlier course).  You can fix this with:

mv ~/.local ~/.local-old

If the output shows evidence of a different uncaught exception, contact the course staff.

""".decode())

#         Traceback (most recent call last):
#   File "/opt/conda/lib/python3.7/site-packages/pkg_resources/__init__.py", line 584, in _build_master
#     ws.require(__requires__)
#   File "/opt/conda/lib/python3.7/site-packages/pkg_resources/__init__.py", line 901, in require
#     needed = self.resolve(parse_requirements(requirements))

# During handling of the above exception, another exception occurred:

# Traceback (most recent call last):
#   File "/opt/conda/bin/cse142", line 6, in <module>
#     from pkg_resources import load_entry_point
#   File "/opt/conda/lib/python3.7/site-packages/pkg_resources/__init__.py", line 3254, in <module>
#     @_call_aside
#   File "/opt/conda/lib/python3.7/site-packages/pkg_resources/__init__.py", line 3238, in _call_aside
#     f(*args, **kwargs)
#   File "/opt/conda/lib/python3.7/site-packages/pkg_resources/__init__.py", line 3267, in _initialize_master_working_set
#     working_set = WorkingSet._build_master()
#   File "/opt/conda/lib/python3.7/site-packages/pkg_resources/__init__.py", line 586, in _build_master
#     return cls._build_from_requirements(__requires__)
#   File "/opt/conda/lib/python3.7/site-packages/pkg_resources/__init__.py", line 599, in _build_from_requirements
#     dists = ws.resolve(reqs, Environment())
#   File "/opt/conda/lib/python3.7/site-packages/pkg_resources/__init__.py", line 787, in resolve
#     raise DistributionNotFound(req, requirers)
# pkg_resources.DistributionNotFound: The 'ipython-genutils' distribution was not found and is required by jupyter-contrib-nbextensions, traitlets


def plot1(file=None, df=None, field="per_element"):
    if df is None:
        df = render_csv(file)
    std=df[field].std()
    mean=df[field].mean()
    max_v =df[field].max()
    min_v =df[field].min()
    fix, axs = plt.subplots(1,2)
    axs[0].set_ylabel("per_element")
    axs[1].set_ylabel("Count")
    df[field].plot.line(ax=axs[0])
    df[field].plot.hist(ax=axs[1])
    axs[0].set_ybound(0,max_v*2)
    axs[0].set_xlabel("rep")
    axs[1].set_xlabel("per_element") 
    axs[1].set_xbound(1e-8,3e-8)
    plt.tight_layout()
    
    max_error = max(abs(max_v-mean),abs(min_v-mean))
    
    return pd.DataFrame.from_dict(dict(mean_per_rep=[df['ET'].mean()], mean=[mean], max=[max_v], min=[min_v], max_error_percent=[(max_error)/mean*100.0]))
    #f"Mean: {mean:1.2}; std: {std:1.2}; max: {max:1.2}; min: {min:1.2}; range: {max-min:1.2}; max-variation: {(max-min)/mean*100.0:1.2f}%"


from IPython.display import display, Markdown, Latex, Code, HTML
import re

def fiddle(fname, function=None, compile=True, code=None, opt=None, run=True, gprof=False, build_cmd=None, exe=None, **kwargs):
    if not run:
        gprof=False
    if gprof:
        run = True
    if not compile:
        run = False
    if opt is None:
        opt = ""

    if code is not None:
        if os.path.exists(fname):
                i = 0
                root, ext = os.path.splitext(fname)
                while True:
                    p = f"{root}_{i:04}{ext}"
                    if not os.path.exists(p):
                        break
                    i += 1
                os.rename(fname, p)

        
        with open(fname, "w") as  f:
            f.write(code)
    
    base, _ =  os.path.splitext(fname)

    if exe is None:
        exe = f"{base}.exe"
        
    if compile:
        print("Compiling your code...")
        print("=====================================")
        if build_cmd is None:
            shell_cmd(f"rm -f build/{base}.s {base}.exe build/{base}.o")
            shell_cmd(f"make build/{base}.s {exe} C_OPTS='{opt}' {'GPROF=yes' if gprof else ''}", shell=True)
        else:
            shell_cmd(build_cmd, shell=True)
            
        if run:
            print("Running your code...")
            print("=====================================")
            shell_cmd(f"./{exe}")

    if function is not None or gprof:
        return build_reps(src=fname, asm=f"build/{base}.s", obj=exe, gprof=gprof, function=function, run=run, **kwargs)
    else:
        return None


def compare(content):
    return HTML("""
            <style>
        .side-by-side {
            display: flex;
            align-items: stretch;

        }
        .side-by-side-pane {
            margin-right:1em;
            border-right-style: solid;
            border-right-color: black;
            border-right-width: 1px;
            flex: 1;
        }
        </style>
        <div class="side-by-side"> """ +
        "".join([f"<div class='side-by-side-pane'>{c._repr_html_()}</div>" for c in content]) +
                """
        </div>
    """)

def render_code(*argc, **kwargs):
    display(do_render_code(*argc, **kwargs))
def do_render_code(file, lang="c++", show=None, trim_ends=False):
    lines = open(file).readlines()
    start_line = 0
    end_line = len(lines)
        
    if isinstance(show, str):
        show = (f"[\s\*]{show}\s*\(", "^\}")
    
    if show is not None and len(show) == 2:
        if isinstance(show[0], str): # regexes
            started = False
            for n, l in enumerate(lines):
                if not started:
                    if re.search(show[0], l):
                        start_line = n
                        started = True
                else:
                    if re.search(show[1], l):
                        end_line = n + 1
                        break
        else:
            start_line, end_line = show
    elif show is not None:
        raise ValueError(f"{show} is not a good range for displaying code")
    
    if trim_ends:
        start_line += 1
        end_line -= 1
            
    comments = {"c++": "//",
                "gas": ";",
                "python" : "#"}
    
    src = f"{comments.get(lang, '//')} {file}:{start_line+1}-{end_line} ({end_line-start_line} lines)\n"
    src += "".join(lines[start_line:end_line])

    return Code(src, language=lang)

import pandas as pd

def render_csv(file, columns = None, sort_by=None, average_by=None, skip=0):
    df = pd.read_csv(file, sep=",")
    df = df[skip:]
    if sort_by:
        df = df.sort_values(by=sort_by)
    if average_by:
        df = df.groupby(average_by).mean()
        df[average_by] = df.index
    if columns:
        df = df[columns]
        
    return df

def _(csv, key, row, column, average_by=None):
    df = render_csv(csv, average_by=average_by)
    return df.filter(like="row", axis=key)[column]
    
    
from contextlib import contextmanager

@contextmanager
def layout(subplots, columns=4):
    f = plt.figure()
    rows = int(math.ceil(subplots/columns))
    f.set_size_inches(4*columns, 4*rows)
    def next_fig():
        c = 0
        while True:
            c += 1
            assert c <= subplots, "Too many subplots.  Increase the number you passed to layout()"
#            f.set_size_inches(4*columns, 4*rows)
            yield f.add_subplot(rows, columns, c)

    try:
        yield f, next_fig()
    finally:
        f.figsize=[4*columns, 4*rows]
        plt.tight_layout()
        

import matplotlib.pyplot as plt
def plot2(file=None, df=None, field="per_element"):
    if df is None:
        df = render_csv(file)

    df['incremental_average'] = incremental_average(df['per_element'])
    df['incremental_error'] = (df['per_element'].mean() - df['incremental_average'])/df['per_element'].mean()*100
    df['rep_error'] = (df['per_element'] - df['incremental_average'])/df['per_element'].mean()*100
    print(f"===================================\nMean = {df['per_element'].mean()}\nTotal execution time = {df['ET'].sum()}")

    std = df[field].std()
    mean = df[field].mean()
    max_v = df[field].max()
    min_v = df[field].min()
    
    with layout(subplots=1) as (fig, sub):
        axs = next(sub)
        
        axs.set_ylabel("per_element runtime")
        axs.set_ybound(0, df['per_element'].max()*2)
        axs.set_xlabel("rep")
        axs.set_title("Per-element time for each rep")
        df["per_element"].plot.line(ax=axs)
        axs.plot([0,200], [df["per_element"].mean(),df["per_element"].mean()], label="mean")
        axs.legend()
        return 
        
        axs = next(sub)
        axs.set_ylabel("absolute error (%)")
        axs.set_ybound(0,df['rep_error'].max()*2)
        axs.set_xlabel("rep")
        axs.set_title("Error compared to mean across all reps")
        df["rep_error"].abs().plot.line(ax=axs)

        axs.set_ylabel("absolute error (%)")
        axs.set_ybound(0,df['rep_error'].max()*2)
        axs.set_xlabel("rep")
        axs.set_title("Error compared to mean across all reps")
        df["rep_error"].abs().plot.line(ax=axs)

        axs = next(sub)
      
        axs.set_ylabel("incremental_average")
        axs.set_ybound(0,df['incremental_average'].max()*2)
        axs.set_xlabel("N")
        axs.set_title("Average over the first N reps")
        df["incremental_average"].plot.line(ax=axs)

        axs = next(sub)
        axs.set_ylabel("absolute error (%)")
        axs.set_ybound(0,df['incremental_error'].max()*2)
        axs.set_xlabel("rep")
        df["incremental_error"].abs().plot.line(ax=axs)

        axs = next(sub)
        axs.set_ylabel("Count")
        axs.set_xlabel("per_element") 
        axs.set_xbound(1e-8,3e-8)
        df["per_element"].plot.hist(ax=axs)



def plotPE(file=None, what=None, df=None,  lines=False, columns=4, logx=None, logy=None, average=False, average_by=None, dot_size=5, combined=False, colorful=True):

    if df is None:
        df = render_csv(file,average_by=average_by)

    colors = ["blue",
              "red",
              "green",
              "purple",
              "orange",
              "yellow"
        ]
    axs = None
    with layout(subplots=len(what), columns=columns) as (fig, sub):
        for i, (x, y) in enumerate(what):
            d = df.copy()
            if not combined or not axs:
                axs = next(sub)
            axs.set_ylabel(y)
            axs.set_xlabel(x)
            axs.set_title(y)

            if logx:
                axs.set_xscale("log", basex=logx)
            else:
                axs.set_xbound(0, d[x].max()*1.3)
                axs.set_autoscalex_on(False)

            if logy:
                axs.set_yscale("log", basey=logy)
            else:
                axs.set_ybound(0, d[y].max()*1.3)
                axs.set_autoscaley_on(False)

                
            if average:
                d = d.groupby(x).mean()
                d[x] = d.index
                #axs.plot.errorbar(x, y, yerr=1, fmt="o")
                d.plot.scatter(y=y, x=x, ax=axs, s=dot_size, c=colors[i % len(colors) if colorful else 1])

                if lines:
                    d.plot(y=y, x=x, ax=axs, c=colors[i % len(colors)] )

            else:
                d.plot.scatter(y=y, x=x, ax=axs, s=dot_size, c=colors[i % len(colors) if colorful else 1])
                if lines:
                    d.plot(y=y, x=x, ax=axs, c=colors[i % len(colors)])

def _plotPE(file=None, what=None, df=None, columns=4, log=False, average=False, average_by=None, dot_size=5):
    if df is None:
        df = render_csv(file,average_by=average_by)

    with layout(subplots=len(what), columns=columns) as (fig, sub):
        for i, (x, y) in enumerate(what):
            d = df.copy()
            axs = next(sub)
            axs.set_ylabel(y)
            axs.set_xlabel(x)
            axs.set_title(y)
            if log:
                axs.set_xscale("log")
                axs.set_yscale("log")
            else:
                axs.set_ybound(0, d[y].max()*1.3)
                axs.set_xbound(0, d[x].max()*1.3)
                axs.set_autoscalex_on(False)
                axs.set_autoscaley_on(False)

                
            if average:
                d = d.groupby(x).mean()
                d[x] = d.index
                #axs.plot.errorbar(x, y, yerr=1, fmt="o")
                d.plot.scatter(y=y, x=x, ax=axs, s=dot_size)

            else:
                d.plot.scatter(y=y, x=x, ax=axs, s=dot_size)

def plotPEBar(file, what, columns=4, log=False, average=False, average_by=None, skip=0):
    rows = int(math.ceil(len(what)/columns))
    f = plt.figure(figsize=[4*columns, 4*rows], dpi = 100)
    
    for i, (x, y) in enumerate(what):
        df = render_csv(file, average_by=average_by, skip=skip)
        axs = f.add_subplot(rows, columns, i+ 1)
        axs.set_ylabel(y)
        #axs.set_xlabel(x)
        axs.set_title(y)
        if log:
            #axs.set_xscale("log")
            axs.set_yscale("log")
        else:
            axs.set_ybound(0, df[y].max()*1.4)
            #axs.set_xbound(0, df[x].max()*1.1)
            axs.set_autoscalex_on(False)
            axs.set_autoscaley_on(False)
            
        if average:
            df = df.groupby(x).mean()
            df[x] = df.index
            df.plot.bar(y=y, x=x, ax=axs)
        else:
            df.plot.bar(y=y, x=x, ax=axs)
        
        for i, v in enumerate(df[y]):
            axs.text(i, v, f"{float(v):3.2}", ha='center' )
            
    plt.tight_layout()

    

            
def incremental_average(d):
    return [d[0:i+1].mean() for i in range(len(d))]

#plot2("200_first_try.csv")

def IC_avg_and_combine(*argc):
    all = render_csv(argc[0])
    all = all[0:0]

    for f in argc:
        df = render_csv(f)
        t = df['function'][0]
        r = dict(function=df['function'][0],
                 reps=int(f.split(".")[0]),
                 IC=df['IC'].sum(),
                 CPI=df['CPI'].mean(),
                 CT=df['CT'].mean(),
                 ET=df['ET'].sum(),
                 MHz=df['MHz'].mean())
        all = all.append(r, ignore_index=True)

    return all


@click.command()
@click.option("--out",  "-o", help="Output file", show_default=True)
@click.argument("notebook", nargs=1,type=click.File("r"))
def nbturnin(notebook=None, out=None):

    nb = nbformat.read("Assignment.ipynb", as_version=nbformat.NO_CONVERT)

    in_answer_section = 0
    
    for cell_index, cell in reversed(list(enumerate(nb.cells))): # in reverse so we can delete in place.
        cell = nb.cells[cell_index]
        
        if "solution2" in cell.metadata:
            del cell.metadata["solution2"]
            
        if "solution2_first" in cell.metadata:
            del cell.metadata["solution2_first"]
            
        if cell.metadata.get("cs203.is_answer_start", False): 
            in_answer_section -= 1  # the sign is backward because we process the notebook in reverse.
        if cell.metadata.get("cs203.is_answer_end", False):
            in_answer_section += 1
            
        if cell.metadata.get('cs203.is_response', False) or in_answer_section > 0:
            pass
        else:
            del nb.cells[cell_index]


    if out is None:
        out = ".".join(notebook.name.split(".")[:-1] + ["turnin", "ipynb"])
    with open(out, "w") as f:
        nbformat.write(nb, f, version=nbformat.NO_CONVERT)

    click.echo(f"Wrote to {out}")
    
