import sys
import os
import os.path

deployment_title = """
<HTML>
<HEAD>
<TITLE>$deployment Deployment</TITLE>
</HEAD>
<h1>$deployment Deployment Documentation</h1>
</HTML>
"""

deployment_doc = """
<HTML>
<HEAD>
<TITLE>$deployment Deployment</TITLE>
</HEAD>
<FRAMESET rows="50, 400">
  <FRAME src="deployment_title.html">
  <FRAMESET cols="30%, 70%">
    <FRAME src="module_nav.html">
    <FRAME name="current_desc" src="sdd.html">
  </FRAMESET>
</FRAMESET>
</HTML>
"""

nav_doc_start = """
<HTML>
<HEAD>
<TITLE>$deployment Module Navigation</TITLE>
</HEAD>
<style>
table, th, td {
    border-collapse: collapse;
}
th, td {
    padding: 5px;
}
th {
    text-align: left;
}
html {
    font-family: Arial, sans-serif ;
    font-size: 90% ;
}
</style>

<table style="width:100%">
  <tr>
    <td>$deployment deployment SDD</td> 
    <td><a href="sdd.html" target="current_desc">HTML</a></td> 
    <td><a href="sdd.md" target="current_desc">MD</a></td> 
  </tr>
</table>
<hr>
<table style="width:100%">
  <tr>
    <th><u><b>Module</b></u></th>
    <th><u><b>Doc</b></u></th> 
  </tr>
"""

md_title = '''
<title>$deployment Application Navigation</title>
# $deployment Deployment

## SDD Links

### Deployment

[$deployment SDD](sdd.md)

### Modules
|Module|SDD|Dictionary|
|---|---|---|
'''

md_doc_line ='''|%s|[Link](%s)|[Link](%s)|
'''


# nav_doc_line = """
#   <tr>
#     <td>%s</td>
#     <td><a href="%s" target="current_desc">SDD</a></td> 
#     <td><a href="%s" target="current_desc">DOX</a></td> 
#     <td><a href="%s" target="current_desc">SCRUB</a></td> 
#   </tr>
# """

nav_doc_line = """
  <tr>
    <td>%s</td>
    <td><a href="%s" target="current_desc">HTML</a></td> 
    <td><a href="%s" target="current_desc">MD</a></td> 
  </tr>
"""

nav_doc_end = """  
</table>
</HTML>
"""

if len(sys.argv) < 3:
    print(("%s: Must specify dest file and one module!"%sys.argv[0]))
    
path_to_dep = "%s/%s"%(os.environ["BUILD_ROOT"],sys.argv[1]) 
print("Generating HTML files for %s" % path_to_dep)

open("%s/docs/deployment_title.html"%path_to_dep,"w").write(deployment_title.replace("$deployment",sys.argv[1]))
open("%s/docs/deployment.html"%path_to_dep,"w").write(deployment_doc.replace("$deployment",sys.argv[1]))

nav_doc = open("%s/docs/module_nav.html"%path_to_dep,"w")
nav_doc.write(nav_doc_start.replace("$deployment",sys.argv[1]))

md_doc = open("%s/docs/%s.md"%(path_to_dep,sys.argv[1]),"w")
md_doc.write(md_title.replace("$deployment",sys.argv[1]))

for module in sys.argv[2:]:
    nav_doc.write(nav_doc_line % (module,
       "../../" + module + "/docs/sdd.html",
       "../../" + module + "/docs/sdd.md")
    )
    md_doc.write(md_doc_line % (module,
       "../../" + module + "/docs/sdd.md",
       "../../" + module + "/docs/" + os.path.basename(module) + ".md"
       )
    )
#       "../../" + module +"/docs/dox.html",
#       "../../" + module +"/docs/scrub.html"))

nav_doc.write(nav_doc_end)