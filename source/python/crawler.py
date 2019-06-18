#!/usr/bin/python
#
# This program crawls the specified site and generates a text to represent it.
#
# This collects internal nodes only.
#
# Usage: crawler.py url [limit] [i|d|e]
#  The option ''url'' is the start page of the crawling.
#  The crawling continues while it can find unvisited pages on the same host.
#  The option ''limit'' specifies the depth of jumping links.
#   (-1 means infinite jump-depth)
#  The option i, d, or e specifies the region of the output graph.
#   i: internal nodes only
#   d: descendant nodes only
#   e: external nodes with 'Out of domain'
#
#  Example usage:
#   python crawler.py http://www.ipl.t.u-tokyo.ac.jp/ > result.txt
#      The resulting web graph contains all pages on www.ipl.t.u-tokyo.ac.jp
#      that can be reached from the top page by hyper-links.
#
#   python crawler.py http://www.ipl.t.u-tokyo.ac.jp/ 3 > result.txt
#      The resulting web graph contains all pages on www.ipl.t.u-tokyo.ac.jp
#      that can be reached from the top page by less than 4 hyper-link jumps.
#
#   python crawler.py http://www.ipl.t.u-tokyo.ac.jp/~emoto/ > result.txt
#      The resulting web graph contains all pages on www.ipl.t.u-tokyo.ac.jp
#      that can be reached from the page "/~emoto/", which may include the top
#      page.
#
#   python crawler.py http://www.ipl.t.u-tokyo.ac.jp/~emoto/ -1 d > result.txt
#      The resulting web graph contains all pages under the page "/~emoto/".
#

import urllib
import urllib2
import urlparse
import re
import sys

if len(sys.argv) < 2:
    print "crawler.py url [limit] [i|d|e]"
    exit(0)

url = sys.argv[1]
limit = -1
if len(sys.argv) > 2:
    limit = int(sys.argv[2])

fDescentOnly = len(sys.argv) > 3 and sys.argv[3] == "d"
fInternalOnly = len(sys.argv) > 3 and sys.argv[3] == "i"

#url="http://www.ipl.t.u-tokyo.ac.jp/"
re_href = re.compile('<\\s*[\\w]+\\s+href\\s*=\\s*(["\'][^>\'"]+["\']|[^ >]+)')
re_src = re.compile('<\\s*[\\w]+\\s+src\\s*=\\s*(["\'][^>\'"]+["\']|[^ >]+)')
re_refresh = re.compile('<[^>]*refresh[^>]*CONTENT=["\'][0-9]*;URL=([^\'"><\\s]*)')
re_quote = re.compile('["\'\\s]*([^>\'"]+)["\'\\s]*')
re_mailto = re.compile('^mailto')
re_title = re.compile('<\\s*[Tt][iI][Tt][Ll][Ee]\\s*>([^<]*)')
re_error = re.compile('^error')

def clean(href):
    m = re_quote.match(href)
    if m: return href[m.start(1):m.end(1)]
    return href

def valid(href):
    if re_mailto.match(href): return False
    return True

def normalize(base, href):
    parsedurl = urlparse.urlparse(href)
    href = urlparse.urlunparse((parsedurl[0], parsedurl[1], parsedurl[2], parsedurl[3], parsedurl[4], "")) # remove #hogehoge
    if parsedurl[1] == '':
        href=urlparse.urljoin(base, href)
    elif parsedurl[2] == '':
        href=urlparse.urlunparse((parsedurl[0],parsedurl[1],"/",parsedurl[3],parsedurl[4],""))
    return urllib.unquote(href)

def ishtml(mtype):
    return mtype == 'text/html' or mtype == 'application/xhtml+xml'

def stripNewline(str):
    return "".join([ c for c in str if not (c in "\n\r")])
def extractURLs(url):
    try:
        f = urllib2.urlopen(url)
        info = f.info()
        mtype = info.gettype( )
        if ishtml(mtype):
            cont = f.read()
            hrefs = [ clean(href) for href in re_href.findall(cont) + re_src.findall(cont) + re_refresh.findall(cont) ]
            urls = [ normalize(url, href) for href in hrefs if valid(href) ]
            m = re_title.search(cont)
            if m:
                title = cont[m.start(1):m.end(1)]
            else:
                title = ""
        else:
            urls = []
            title= ""
        return (url, urls, mtype, stripNewline(title))
    except IOError, e:
        if hasattr(e, 'code'):
            return (url, [], "error %d" % e.code, "")
        return (url, [], "error", "")

def visit(baseurl, allowedhosts, url):
    parsedu = urlparse.urlparse(url)
    if fDescentOnly: 
        if isDescent(baseurl, url):
            sys.stderr.write("visiting %s\n" % url)
            ret = extractURLs(url)
        else:
            sys.stderr.write("skipping %s\n" % url)
            ret = (url, [], "Out of range", "Out of range")
    elif fInternalOnly:
        if isInternal(allowedhosts, url):
            sys.stderr.write("visiting %s\n" % url)
            ret = extractURLs(url)
        else:
            sys.stderr.write("skipping %s\n" % url)
            ret = (url, [], "Out of range", "Out of range")
    else:
        if parsedu[1] in allowedhosts:
            sys.stderr.write("visiting %s\n" % url)
            ret = extractURLs(url)
        else:
            sys.stderr.write("skipping %s\n" % url)
            ret = (url, [], "Out of domain", "Out of domain")
    return ret

def isInternal(allowedhosts, url):
    parsedu = urlparse.urlparse(url)
    return parsedu[1] in allowedhosts

def isDescent(beginurl, url):
    return url.startswith(beginurl)


# allurls :: index -> urls
# indices :: url -> index

def crawl(url, maxdepth):
    parsedurl = urlparse.urlparse(url)
    allowedhosts = [parsedurl[1]]
    indices = {}
    allurls = []
    queue = [normalize(url,url)]
    depth = 0
    visited = {}
    indices[queue[0]] = 0
    allurls += [()]
    while len(queue) > 0 and not (depth == maxdepth):
        nextqueue = []
        for i, u in enumerate(queue):
            if not (u in visited):
                visited[u] = len(visited)
                ret = visit(url, allowedhosts, u)
                if not re_error.match(ret[2]):
                    for v in ret[1]:
                        if not (v in indices):
                            indices[v] = len(indices)
                            allurls+=[(v, [], "Out of Range", "")]
                            nextqueue += [v]
                allurls[indices[u]] = (ret[0], [indices[v] for v in ret[1]], ret[2], ret[3])
        queue = nextqueue
        depth += 1
    return (allurls, indices)


def myquote(url):
    pd = urlparse.urlparse(url)
    return urlparse.urlunparse((pd[0], urllib.quote(pd[1]), urllib.quote(pd[2]), urllib.quote(pd[3]), urllib.quote(pd[4]), urllib.quote(pd[5])))
    

def printout(url, allurls, indices, indexmap):
    print url
    s = 0
    for i, ret in enumerate(allurls):
        if indexmap[i] >= 0:
            s = s + 1
    print s
    for i, ret in enumerate(allurls):
        if indexmap[i] >= 0:
            if ishtml(ret[2]):
                print '%d %s "%s"' % (indexmap[i], myquote(ret[0]), ret[3])
            else:
                print '%d %s "%s"' % (indexmap[i], myquote(ret[0]), ret[2])
    s = 0
    for i, ret in enumerate(allurls):
        if indexmap[i] >= 0:        
            for u in ret[1]:
                if indexmap[u] >= 0:
                    s = s + 1
    print s
    for i, ret in enumerate(allurls):
        if indexmap[i] >= 0:        
            for u in ret[1]:
                if indexmap[u] >= 0:
                    print "%d %d" % (indexmap[i], indexmap[u])

def allUrls(url, allurls, indices):
    indexmap=[-1] * len(allurls)
    for i, ret in enumerate(allurls):
        indexmap[i] = i
    return indexmap        

def internalOnly(url, allurls, indices):
    parsedu = urlparse.urlparse(url)
    allowedhosts = [parsedu[1]]
    indexmap=[-1] * len(allurls)
    k=0
    for i, ret in enumerate(allurls):
        if isInternal(allowedhosts, ret[0]):
            indexmap[i] = k
            k = k + 1
        else:
            indexmap[i] = -1
    return indexmap

def descentOnly(url, allurls, indices):
    parsedu = urlparse.urlparse(url)
    allowedhosts = [parsedu[1]]
    indexmap=[-1] * len(allurls)
    nurl = normalize(url,url)
    k=0
    for i, ret in enumerate(allurls):
        if isDescent(nurl, ret[0]):
            indexmap[i] = k
            k = k + 1
        else:
            indexmap[i] = -1
    return indexmap

(allurls, indices) = crawl(normalize(url,url), limit)
#print allurls
#print indices

if fDescentOnly:
    indexmap = descentOnly(url, allurls, indices)
elif fInternalOnly:
    indexmap = internalOnly(url, allurls, indices)
else:
    indexmap = allUrls(url, allurls, indices)

printout(url, allurls, indices, indexmap)


# cat me6.txt | sed -e '1{s/.*/digraph "&" {/p;d};2d;/"/{s/^\([0-9]*\) [^ ]* \(".*"\)$/\1 [label=\2]/p;d};s/ / -> /p;${s/.*/}/p};d ' > me6.dot


# cat me6.txt | sed -e '1{s/.*/digraph "&" {/p;d};2d;/"/{s/^\([0-9]*\) [^ ]* \(".*"\)$/\1 [label=\2]/;d};s/ / -> /p;${s/.*/}/p};d ' > me6.dot
# dot -Tps < me6.dot > me6.ps

# cat me7.txt | sed -e '1{s/.*/digraph "&" {/p;d};2d;/"/{s/^\([0-9]*\) [^ ]* \(".*"\)$/\1 [label=\2]/;d};s/ / -> /p;${s/.*/}/p};d ' > me7.dot
# dot -Tps < me7.dot > me7.ps