import math
from spn import SPN
import sys

class PSDD:
	class Node:
		def __init__(self):
			self.type = ''
			self.id = -1

			self.literal = -1 # for leafs
			self.prob = 1 # 1 for leaf=1, true nodes have different prob

			self.n_decomp = 0 # for D nodes (number of num nodes)
			self.elems = [] # list of all decompositions

		# tab is a pssd line splitted by ' ' (and '\n' removed)
		def parse_psdd_line(self, tab):
			TYPE_INDEX = 0
			ID_INDEX = 1
			VTREE_INDEX = 2
			LITERAL_INDEX = 3
			TRUE_NODE_INDEX = 4
			DECOMP_INDEX = 3

			# print(tab)

			self.id = int(tab[ID_INDEX])
			self.type = tab[0]
			if(self.type == 'L'):
				self.literal = int(tab[LITERAL_INDEX])
			elif(self.type == 'T'):
				self.literal = int(tab[LITERAL_INDEX])
				self.prob = math.exp(float(tab[TRUE_NODE_INDEX]))
			elif(self.type == 'D'):
				self.n_decomp = int(tab[DECOMP_INDEX])
				self.elems = [None] * self.n_decomp
				for i in range(self.n_decomp):
					self.elems[i] = {
						'prime_id':int(tab[DECOMP_INDEX+1+3*i]),
						'sub_id':int(tab[DECOMP_INDEX+1+3*i+1]),
						'prob':math.exp(float(tab[DECOMP_INDEX+1+3*i+2]))}
			else:
				print('Unknown type of node:', tab[0])
				exit()

		def print(self):
			print("================================================")
			print('Id:', self.id)
			print('Type:', self.type)
			if(self.type=='L' or self.type=='T'):
				print('Lit:', self.literal)
			if(self.type=='T'):
				print('Prob:', self.prob)
			if(self.type=='D'):
				print('N_decom', self.n_decomp)
				print(self.elems)


	def __init__(self, filename):
		self.node_list = []
		self.node_count = 0

		with open(filename) as fp:
			for psdd_line in fp:
				line = psdd_line.split('\n')
				tab = psdd_line.split(' ')
				if(tab[0] == 'c'):
					continue
				elif(tab[0] == 'psdd'):
					self.node_count = int(tab[1])
				elif(len(tab) > 1):
					node = PSDD.Node()
					node.parse_psdd_line(tab)
					self.node_list.append(node)

		# safety check:
		if(len(self.node_list) != self.node_count):
			print("Unconsistent psdd file, length do not match")
			# exit()
		for node in self.node_list:
			if(node.id == -1):
				print("Node id is Unconsistent")
				exit()

	def print(self):
		for node in self.node_list:
			node.print()

	def export_spn(self):
		spn_node_list = []

		id_list = []
		for psdd_node in self.node_list:
			id_list.append(psdd_node.id)
		Idg = SPN.Id_generator(id_list)

		for psdd_node in self.node_list:
			if(psdd_node.type=='L'):
				spn_node = SPN.Node()
				spn_node.id = psdd_node.id
				spn_node.type = 'L'
				spn_node.literal = psdd_node.literal

				spn_node_list.append(spn_node)
			elif(psdd_node.type=='T'):
				if(psdd_node.prob == 1): # true node is literal
					spn_node = SPN.Node()
					spn_node.id = psdd_node.id
					spn_node.type = 'L'
					spn_node.literal = psdd_node.literal

					spn_node_list.append(spn_node)
				else: # general case

					spn_node_L1 = SPN.Node()
					spn_node_L1.type = 'L'
					spn_node_L1.id = Idg.gen_id()
					spn_node_L1.literal = psdd_node.literal

					spn_node_W1 = SPN.Node()	# weight node
					spn_node_W1.type = 'W'
					spn_node_W1.id = Idg.gen_id()
					spn_node_W1.elems = [spn_node_L1.id, psdd_node.prob]

					spn_node_L2 = SPN.Node()
					spn_node_L2.type = 'L'
					spn_node_L2.id = Idg.gen_id()
					spn_node_L2.literal = -psdd_node.literal

					spn_node_W2 = SPN.Node()	# weight node
					spn_node_W2.type = 'W'
					spn_node_W2.id = Idg.gen_id()
					spn_node_W2.elems = [spn_node_L2.id, 1-psdd_node.prob]

					spn_node_S = SPN.Node() 	# sum node
					spn_node_S.type = 'S'
					spn_node_S.id = psdd_node.id
					spn_node_S.n_decomp = 2
					spn_node_S.elems = [spn_node_W1.id, spn_node_W2.id]

					spn_node_list.append(spn_node_L1)
					spn_node_list.append(spn_node_L2)
					spn_node_list.append(spn_node_W1)
					spn_node_list.append(spn_node_W2)
					spn_node_list.append(spn_node_S)
			elif(psdd_node.type=='D'):
				if(psdd_node.n_decomp == 1): # 1 input Sum
					if(psdd_node.elems[0]['prob'] == 1): # 2 inputs product
						spn_node_P = SPN.Node()
						spn_node_P.id = psdd_node.id
						spn_node_P.type = 'P'
						spn_node_P.n_decomp = 2;
						spn_node_P.elems = [psdd_node.elems[0]['prime_id'], psdd_node.elems[0]['sub_id']]

						spn_node_list.append(spn_node_P)
					else: # 3 inputs product
						spn_node_P = SPN.Node()
						spn_node_P.id = Idg.gen_id()
						spn_node_P.type='P'
						spn_node_P.n_decomp = 2
						spn_node_P.elems = [psdd_node.elems[0]['prime_id'], psdd_node.elems[0]['sub_id']]

						spn_node_W = SPN.Node()
						spn_node_W.id = psdd_node.id
						spn_node_W.type = 'W'
						spn_node_W.elems = [spn_node_P.id, psdd_node.elems['prob']]

						spn_node_list.append(spn_node_P)
						spn_node_list.append(spn_node_W)
				else: # >= 2 input Sum
					sum_idlist = [] # id of child of sum of decomposition nodes
					for i in range(psdd_node.n_decomp):
						if(psdd_node.elems[i]['prob'] == 1): # no weight node
							spn_node_P = SPN.Node()
							spn_node_P.id = Idg.gen_id()
							spn_node_P.type = 'P'
							spn_node_P.n_decomp = 2;
							spn_node_P.elems = [psdd_node.elems[i]['prime_id'], psdd_node.elems[i]['sub_id']]

							sum_idlist.append(spn_node_P.id)
							spn_node_list.append(spn_node_P)
						else: # 3 inputs product
							spn_node_P = SPN.Node()
							spn_node_P.id = Idg.gen_id()
							spn_node_P.type='P'
							spn_node_P.n_decomp = 2
							spn_node_P.elems = [psdd_node.elems[i]['prime_id'], psdd_node.elems[i]['sub_id']]

							spn_node_W = SPN.Node()
							spn_node_W.id = Idg.gen_id()
							spn_node_W.type = 'W'
							spn_node_W.elems = [spn_node_P.id, psdd_node.elems[i]['prob']]

							sum_idlist.append(spn_node_W.id)
							spn_node_list.append(spn_node_P)
							spn_node_list.append(spn_node_W)

					queue = sum_idlist
					next_queue = []
					while(queue != []):
						if(len(queue)==2 and next_queue==[]):
							id_1 = queue.pop(0)
							id_2 = queue.pop(0)

							spn_node_S = SPN.Node()
							spn_node_S.id = psdd_node.id
							spn_node_S.type = 'S'
							spn_node_S.n_decomp = 2
							spn_node_S.elems = [id_1, id_2]

							spn_node_list.append(spn_node_S)
						elif(len(queue)==1):
							next_queue.append(queue.pop(0))
							queue = next_queue
							next_queue = []
						else:
							id_node_1 = queue.pop(0)
							id_node_2 = queue.pop(0)

							spn_node_S = SPN.Node()
							spn_node_S.id = Idg.gen_id()
							spn_node_S.type = 'S'
							spn_node_S.n_decomp = 2
							spn_node_S.elems = [id_node_1, id_node_2]

							spn_node_list.append(spn_node_S)
							next_queue.append(spn_node_S.id)
							if(queue==[]):
								queue = next_queue
								next_queue = []
			else:
				print("Unknown node type in export spn:", psdd_node.type)
				exit()

		return spn_node_list

if __name__=='__main__':

	if(len(sys.argv) != 3):
		print("Usage:")
		print("python3 psdd.py <filename.psdd> <out_filename.spn>")

	else:
		in_filename = sys.argv[1]
		out_filename = sys.argv[2]

		psdd_obj = PSDD(in_filename)
		# psdd_obj.print()
		spn_obj = SPN()
		spn_obj.node_list = psdd_obj.export_spn()
		spn_obj.print2file(out_filename)