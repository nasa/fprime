import markdown
import sys
import os
# argument list
# 1 - markdown SOURCE
# 2 - HTML dest

source_md = sys.argv[1]
dest_html = sys.argv[2]

print("Converting %s to %s"%(source_md,dest_html))

if not os.path.exists(source_md):
    print(("Markdown file %s does not exist!" % source_md))
    sys.exit(-1)

# try:
#     html = markdown.markdownFromFile(input=source_md,output=dest_html)
# except:
#     print("Error writing HTML file %s"%(output_file))

html = markdown.markdownFromFile(input=source_md,output=dest_html)
