#!/usr/bin/python
# vim : ts=2 : tw=2
from operator import itemgetter, attrgetter
import sys
import json

def bestFit(nodes, io, chunks, tasksPerNode):
  tasks = dict()

  nodes_io = [[x, y] for x, y in zip(nodes, io)]
  nodes_io = sorted(nodes_io, key=itemgetter(1))

  iterator = 0
  node_iterator = 0
  block = [ ]
  for c in chunks:
    block.append(c)
    if (iterator % tasksPerNode == 0 and iterator != 0) or iterator == len(chunks) - 1:
      block = sorted(block)

      # assing block to server
      node_id = nodes_io[node_iterator][0]
      if node_id not in tasks:
        tasks[node_id] = [block]
      else:
        tasks[node_id].append(block)

      node_iterator += 1;
      node_iterator = node_iterator % len(nodes_io)
      block = [ ]

    iterator += 1

  return tasks

def splitRegions(nodes, regionIO):
  regions = [ ]
  regionsIO = [ ]
  #print nodes
  for node in nodes:
    if node % 3 == 0 or len(nodes) == node:
      upperBoundary = len(nodes) - 1 if node + 3 >= len(nodes) else node + 3
      #print node, upperBoundary 
      if (node == upperBoundary):
        regions.append([nodes[node]])
        regionsIO.append([regionIO[node]])
      else:
        regions.append(nodes[node:upperBoundary])
        regionsIO.append(regionIO[node:upperBoundary])

  return regions, regionsIO

def scheduleBasedOnIO(nodes, io_stats, chunks, chunksPerBlock):
  regions, regionsIO = splitRegions(nodes, io_stats)
  #print regions

  distribution = { }

  for i, val in enumerate(regions):
    leader_node = 1 if len(val) == 3 else 0
    regionChunks = chunks[str(val[leader_node])]
    pivot = val[leader_node]
    if len(val) < 3:
      pass
      
    elif pivot == 0:
      regionChunks += chunks[str(pivot+1)]
      regionChunks += chunks[str(len(node)-1)]

    elif pivot == len(nodes) - 1:
      regionChunks += chunks[str(pivot-1)]
      regionChunks += chunks[str(0)]

    else:
      regionChunks += chunks[str(pivot-1)]
      regionChunks += chunks[str(pivot+1)]

    regionChunks = [int (c) for c in regionChunks]
    regionChunks = sorted(regionChunks)

    #print pivot, chunks, "|", regionChunks
    out = bestFit(val, regionsIO[i], regionChunks, chunksPerBlock)
    tmp = distribution.copy()
    tmp.update(out)
    distribution = tmp

  print json.dumps(distribution)

def read_input():
  j = json.load(sys.stdin)
  nodes          = j['nodes']
  io             = j['io']
  chunksPerBlock = int(j['chunksPerBlock'])
  chunks         = j['chunks']


  nodes = [int(node) for node in nodes]
  io    = [float(io_t) for io_t in io]
  #print nodes

  scheduleBasedOnIO(nodes, io, chunks, chunksPerBlock)


read_input()
