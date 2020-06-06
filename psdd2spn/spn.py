import sys
from PySigmoid import *
from math import *


class SPN:
	# Generate new ids to create new sub nodes
	class Id_generator:
		def __init__(self, id_list=[]):
			self.id_list = id_list
			self.last_id_checked = 0

		def gen_id(self):
			while(self.last_id_checked in self.id_list):
				self.last_id_checked = self.last_id_checked+1
			self.id_list.append(self.last_id_checked)
			return self.last_id_checked

	class Node:
		def __init__(self):
			self.type = '' # S - P - W - L
			self.id = -1

			self.literal = -1

			self.n_decomp = 0
			self.elems = []

			# additional element that a node should contain
			self.depth = 0
			self.parents = []

		def parse_spn_line(self, tab):
			TYPE_INDEX = 0
			ID_INDEX = 1
			LITERAL_INDEX = 2
			TRUE_NODE_INDEX = 3
			DECOMP_INDEX = 2

			self.type = tab[TYPE_INDEX]
			self.id = int(tab[ID_INDEX])
			if(self.type=='L'):
				self.literal = int(tab[LITERAL_INDEX])
			elif(self.type=='S' or self.type=='P'):
				self.n_decomp = int(tab[DECOMP_INDEX])
				for i in range(self.n_decomp):
					self.elems.append(int(tab[DECOMP_INDEX+1+i]))
			elif(self.type=='W'):
				self.elems = [int(tab[DECOMP_INDEX]), float(tab[DECOMP_INDEX+1])]
			else:
				print("Unknown spn node:", self.type)

		def print(self):
			print('===================================')
			print('Id:', self.id)
			print('Type:', self.type)
			if(self.type=='L'):
				print('Lit:', self.literal)
			if(self.type=='T'):
				print('Child:', self.child_id)
			if(self.type=='S' or self.type=='P' or self.type=='W'):
				print('n_decomp:', self.n_decomp)
				print(self.elems)

		def print2file(self):
			print(self.type, end=' ')
			print(self.id, end=' ')
			if(self.type=='L' or self.type=='T'):
				print(self.literal, end=' ')
			if(self.type=='W'):
				print(self.elems[0], self.elems[1], end=' ')
			if(self.type=='S' or self.type=='P'):
				print(self.n_decomp, end=' ')
				for i in range(self.n_decomp):
					print(self.elems[i], end=' ')
			print('')


	def __init__(self):
		self.node_list = []
		self.node_count = 0


	def init(self, filename, size=8, expsize=0):
		# posit information
		self.size = size
		self.expsize = expsize
		set_posit_env(size+1, expsize)
		self.pipeline = 10000 # Number of register = Depth

		# FPGA implementation
		self.depth = 1 # Maximum depth of the nodes
		self.pip = [0.5] # pipeline delimitations
		self.input_list = []
		self.output_list = []

		with open(filename) as fp:
			for spn_line in fp:
				line = spn_line.split('\n')
				tab = line[0].split(' ')
				if(tab[0]=='c'):
					continue
				elif(tab[0]=='spn'):
					self.node_count = int(tab[1])
				else:
					spn_node = SPN.Node()
					spn_node.parse_spn_line(tab)
					self.node_list.append(spn_node)

		self.set_parents()
		self.set_inout_list()
		self.set_depth()
		self.set_pip_limits()

		print("SPN initialized with "+str(self.node_count)+" nodes")

	def set_parents(self):
		for spn_node in self.node_list:
			if(spn_node.type != 'L'):
				parent_id = spn_node.id
				child_list = []
				if(spn_node.type=='W'):
					child_list.append(spn_node.elems[0])
				else: # Type = S or P
					child_list = [elem for elem in spn_node.elems]

				for node in self.node_list:
					if(node.id in child_list):
						child_list.remove(node.id)
						node.parents.append(parent_id)
					if(child_list==[]):
						break
		print('Parent lists set')

	def correct_literal_list(self, in_list, lit_list):
		input_list = []
		literal_list = []

		lit = 1
		while (in_list != []):
			if lit in lit_list:
				index = lit_list.index(lit)
				Id = in_list.pop(index)
				lit_list.pop(index)
				input_list.append(Id)
				literal_list.append(lit)
			else:
				Id = -1
				input_list.append(-1)
				literal_list.append(lit)

			if -lit in lit_list:
				index = lit_list.index(-lit)
				Id = in_list.pop(index)
				lit_list.pop(index)
				input_list.append(Id)
				literal_list.append(-lit)
			else:
				Id = -1
				input_list.append(-1)
				literal_list.append(-lit)

			lit = lit+1

		return input_list, literal_list




	def set_inout_list(self):
		self.input_list = [] # reset
		self.output_list = [] # reset
		self.literal_list = []

		input_list_erroned = []
		literal_list_erroned = []

		for spn_node in self.node_list:
			if(spn_node.type=='L'):
				if(spn_node.literal not in literal_list_erroned):
					input_list_erroned.append(spn_node.id)
					literal_list_erroned.append(spn_node.literal)
			if(spn_node.parents==[]):
				self.output_list.append(spn_node.id)

		self.input_list, self.literal_list = self.correct_literal_list(input_list_erroned, literal_list_erroned)

		# print(self.literal_list)
		# print(self.input_list)
		# sort list according to literal
		sorted_input_list = [-1 for _ in self.input_list]
		sorted_literal_list = [0 for _ in self.literal_list]
		for Id, lit in zip(self.input_list, self.literal_list):
			pos = (abs(lit)-1)*2 + int(lit>0)
			# print(Id, lit, pos)
			sorted_input_list[pos] = Id
			sorted_literal_list[pos] = lit
		self.input_list = sorted_input_list
		self.literal_list = sorted_literal_list

		# print('Input and output lists')
		# print(self.input_list, end=' with literal: ')
		# print(self.literal_list)
		# print(self.output_list)

	def get_node(self, index):
		for node in self.node_list:
			if(node.id == index):
				return node
		print('Node not found, id=', index)
		exit()

	def set_depth(self):
		queue = self.input_list.copy()
		next_queue = []

		depth = 0
		while(queue!=[]):
			curr_index = queue.pop(0)
			while(curr_index==-1):
				curr_index = queue.pop(0)
			curr_node = self.get_node(curr_index)
			curr_node.depth = depth # overwrite with higher depth possible
			for parent_id in curr_node.parents:
				if(not parent_id in next_queue):
					next_queue.append(parent_id)

			if(queue==[]):
				queue = next_queue
				next_queue = []
				depth = depth+1

		self.depth = depth
		print('Depths set, from 0 to', self.depth)
		if(self.pipeline > self.depth):
			# print("Too much pipelines compared to depth")
			# print("Overwritting pipeline to", self.depth)
			self.pipeline = self.depth

	def set_pip_limits(self):
		increment = float(self.depth) / (self.pipeline +1.0)
		self.pip = []
		curr_pip = increment
		while(curr_pip < self.depth):
			self.pip.append(curr_pip)
			curr_pip = curr_pip + increment


	def print(self):
		for spn_node in self.node_list:
			spn_node.print()

	def print2file(self, filename):
		with open(filename, 'w+') as fp:
			orig_stdout = sys.stdout
			sys.stdout = fp

			print('c This is the SPN header')
			print('c')
			print('c file syntax:')
			print('c psdd count-of-nodes')
			print('c L id-of-literal-sdd-node literal')
			print('c S id-of-sum-node number-of-decomposition {id}*')
			print('c W id-of-weight-node child_id weight-value')
			print('c P id-of-product-node {id1 id2}')
			print('c')
			print('spn', len(self.node_list))

			for spn_node in self.node_list:
				spn_node.print2file()

			sys.stdout = orig_stdout
			fp.close()

	def get_literal_from_id(spn_node_list, Id):
		for spn_node in spn_node_list:
			if(spn_node.type=='L' and spn_node.id == Id):
				return spn_node.literal
		print('Node not found :(')
		print('Was looking for id=',Id)
		print(spn_node_list)
		return 0


	def export_sv(self, filename):
		f = open(filename, 'w+')
		orig_stdout = sys.stdout
		sys.stdout = f

		self.generate_sv()

		sys.stdout = orig_stdout
		f.close()

	def print(txt, indent=0):
		for i in range(indent):
			print('\t', end='')
		print(txt)

	def generate_sv(self):

		self.generate_header()
		self.generate_module_definition()
		self.generate_signals_definition()
		self.generate_valid_pipeline()
		self.generate_assign_input_output()
		self.generate_operators()
		self.generate_assign_output()
		self.generate_endmodule()


	def generate_header(self, indent=0):
		SPN.print("`timescale 1ns / 1ps")
		SPN.print("////////////////////////////////////////////////////////////////////////////////")
		SPN.print("// Company: KU Leuven - MICAS departement")
		SPN.print("// Engineer: Antoine Gennart")
		SPN.print("//")
		SPN.print("// Created from script: psdd/spn.py")
		SPN.print("// Design Name: SUM PRODUCT NETWORK (SPN)")
		SPN.print("// Module Name: spn")
		SPN.print("// Project Name: Efficient creation of sum and product network over FPGA")
		SPN.print("// Target Devices: Zed board")
		SPN.print("// Tool Versions:")
		SPN.print("// Description:")
		SPN.print("//")
		SPN.print("// Dependencies:")
		SPN.print("//")
		SPN.print("////////////////////////////////////////////////////////////////////////////////\n")
		SPN.print("`include \"posit.vh\"\n");


	def generate_module_definition(self, indent=0):
		SPN.print("module spn(", indent)
		SPN.print('input clk,', indent+1)
		SPN.print('input rst,', indent+1)
		SPN.print('input enbl_in,', indent+1)
		SPN.print('output ready_in,', indent+1)
		SPN.print('input enbl_out,', indent+1)
		SPN.print('output valid_out,', indent+1)
		SPN.print("input [OUT_WIDTH-1:0] spn_input,", indent+1)
		SPN.print("output [SIZE-1:0] spn_output", indent+1)

		SPN.print(");\n", indent)

	def generate_signal_definitions(name_list, size=1, indent=0):
		if(size==1):
			print('logic ', end=' ')
			for name in name_list[0:-1]:
				print(name, end=', ')
			print(name_list[-1]+';')
		else:
			print('logic [SIZE-1:0]', end=' ')
			for name in name_list[0:-1] :
				print(name, end=', ')
			print(name_list[-1]+';')

	def generate_signal_pipeline(self, parent_depth, child_id_list, parent_id):
		for child_id in child_id_list:
			child = self.get_node(child_id)
			child_depth = child.depth
			pipeline_crossed = [(child_depth <= reg and parent_depth > reg)for reg in self.pip]
			for i in range(sum(pipeline_crossed)):
				name = 'output_'+str(parent_id)+'_'+str(child_id)+'_pip'+str(i)
				SPN.generate_signal_definitions([name], size='SIZE', indent=0)


	def generate_signals_definition(self, indent=0):
		# for input_id in self.input_list:
		# 	name = 'output_N_'+str(input_id)
		# 	SPN.generate_signal_definitions([name], size='SIZE', indent=indent)
		for spn_node in self.node_list:
			if spn_node.type=='L':
				name = 'output_N_'+str(spn_node.id)
				SPN.generate_signal_definitions([name], size='SIZE', indent=indent)
			if spn_node.type=='W':
				name_in1 = 'input_'+str(spn_node.id)+'_'+str(spn_node.elems[0])
				self.generate_signal_pipeline(spn_node.depth, [spn_node.elems[0]], spn_node.id)
				name_out = 'output_N_'+str(spn_node.id)
				SPN.generate_signal_definitions([name_in1, name_out], size='SIZE', indent=indent)
			if spn_node.type=='P' or spn_node.type=='S':
				name_in1 = 'input_'+str(spn_node.id)+'_'+str(spn_node.elems[0])
				name_in2 = 'input_'+str(spn_node.id)+'_'+str(spn_node.elems[1])
				name_out = 'output_N_'+str(spn_node.id)
				self.generate_signal_pipeline(spn_node.depth, spn_node.elems, spn_node.id)
				SPN.generate_signal_definitions([name_in1, name_in2, name_out], size='SIZE', indent=indent)
		print('')

		signals = []
		for pip in range(self.pipeline):
			signals.append('valid_'+str(pip))

		SPN.generate_signal_definitions(signals, size=1)
		print('\n')

	def generate_valid_pipeline(self, indent=0):
		SPN.print("always_ff @(posedge clk) begin", indent)
		SPN.print("if (rst==1'b1) begin", indent+1)
		for i in range(self.pipeline):
			next_name = 'valid_'+str(i)
			SPN.print(next_name+' <= '+"1'b0"+';', indent+2)
		SPN.print("end else if (enbl_in==1'b1) begin", indent+1)
		curr_name = "1'b1"
		for i in range(self.pipeline):
			next_name = 'valid_'+str(i)
			SPN.print(next_name+' <= '+curr_name+';', indent+2)
			curr_name = next_name
		SPN.print("end else  begin", indent+1)
		curr_name  ="1'b0"
		for i in range(self.pipeline):
			next_name = 'valid_'+str(i)
			SPN.print(next_name+' <= '+curr_name+';', indent+2)
			curr_name = next_name;
		SPN.print("end", indent+1)
		SPN.print("end", indent)
		SPN.print('', indent)


	def generate_assign(name1, name2, indent=0):
		SPN.print('assign '+name1+' = '+name2+';', indent)

	def generate_assign_input_output(self, indent=0):
		i = 0
		for spn_node in self.node_list:
			if(spn_node.type=='L'):
				name1 = 'output_N_'+str(spn_node.id)
				name2 = 'spn_input['+str(self.literal_list.index(spn_node.literal))+'] << (SIZE-1)'
				SPN.generate_assign(name1, name2)
				print("// Above literal is "+str(spn_node.literal))
		print("")

	def generate_operation(name, operation,  in_a, in_b, out, indent=0):
		SPN.print(operation+' #(.SIZE(SIZE), .EXP_SIZE(EXP_SIZE)) '+name+' (', indent)
		SPN.print('.op_a( '+in_a+' ),', indent+1)
		SPN.print('.op_b( '+in_b+' ),', indent+1)
		SPN.print('.op_res( '+out+' )', indent+1)
		SPN.print(');', indent)
		SPN.print("", indent)



	def generate_assign_pipeline(self, parent_depth, parent_id, child_id, indent=0):
		child = self.get_node(child_id)
		child_depth = child.depth
		pipeline_crossed = [(child_depth <= reg and parent_depth > reg) for reg in self.pip]
		P = sum(pipeline_crossed)
		if(P==0):
			name1 = 'input_'+str(parent_id)+'_'+str(child_id)
			name2 = 'output_N_'+str(child_id)
			SPN.generate_assign(name1, name2, indent)
		else:
			curr_name = 'output_N_'+str(child_id)
			last_name = 'input_'+str(parent_id)+'_'+str(child_id)
			SPN.print('always_ff @(posedge clk) begin', indent)
			# SPN.print("if (enbl_in==1'b1) begin", indent+1)
			SPN.print('if(rst==0) begin')
			for i in range(P):
				next_name = 'output_'+str(parent_id)+'_'+str(child_id)+'_pip'+str(i)
				SPN.print(next_name+' <= '+curr_name+';', indent+1)
				curr_name = next_name
			SPN.print('end else begin')
			curr_name = 'output_N_'+str(child_id)
			for i in range(P):
				next_name = 'output_'+str(parent_id)+'_'+str(child_id)+'_pip'+str(i)
				SPN.print(next_name+' <= 0;', indent+1)
				curr_name = next_name
			SPN.print('end')
			# SPN.print('end', indent+1)
			SPN.print('end', indent)
			SPN.generate_assign(last_name, curr_name, indent)


	def generate_operator(self, node, indent=0):
		operation = ''
		name = ''
		in_a = ''
		in_b = ''
		if(node.type == 'W'):
			operation = 'multiplier'
			name = 'weight_'+str(node.id)
			in_a = 'input_'+str(node.id)+'_'+str(node.elems[0])
			in_b = self.float2posit(node.elems[1])
			self.generate_assign_pipeline(node.depth, node.id, node.elems[0])

		elif(node.type == 'P' or node.type=='S'):
			operation = 'adder' if node.type=='S' else 'multiplier'
			name = operation+'_'+str(node.id)
			in_a = 'input_'+str(node.id)+'_'+str(node.elems[0])
			in_b = 'input_'+str(node.id)+'_'+str(node.elems[1])
			self.generate_assign_pipeline(node.depth, node.id, node.elems[0])
			self.generate_assign_pipeline(node.depth, node.id, node.elems[1])

		out = 'output_N_'+str(node.id)
		SPN.generate_operation(name, operation, in_a, in_b, out, indent=0)

	def generate_operators(self, indent=0):
		for node in self.node_list:
			if(node.type=='W' or node.type=='P' or node.type=='S'):
				self.generate_operator(node)


	def generate_assign_output(self,indent=0):
		# if(len(self.output_list) != 1):
			# print("Warning : Number of output bigger than 1...")
			# print(len(self.output_list))
			# print(self.output_list)
			# exit(-1)
		output_id = self.output_list[-1]
		SPN.generate_assign('spn_output', 'output_N_'+str(output_id), indent)
		SPN.generate_assign('valid_out', 'valid_'+str(self.pipeline-1), indent)
		# SPN.generate_assign('ready_in', '(~valid_out | enbl_out)', indent)
		SPN.generate_assign('ready_in', "rst ? 1'b0 : 1'b1", indent)

	def generate_endmodule(self, indent=0):
		SPN.print('endmodule', indent)


	def float2posit(self, floatNumber):
		pos = Posit(floatNumber)
		sign_a, regime_a, exponent_a, fraction_a = pos.decode()

		i = 0
		output = ""

		# if(sign_a==1):
		# 	output = output+"1"
		# else:
		# 	output = output+"0"
		# i=i+1

		if(regime_a >= 0):
			while (regime_a >= 0):
				output = output+"1"
				i = i+1
				regime_a = regime_a - 1
			output = output+"0"
			i=i+1
		else:
			while (-regime_a > 0):
				output = output+"0"
				i=i+1
				regime_a = regime_a+1
			output = output+"1"

		j=0
		exp = ""
		while(j < self.expsize):
			div = exponent_a % 2
			exponent_a = exponent_a // 2
			if(div==0):
				exp = "0"+exp
			else:
				exp = "1"+exp
			j=j+1

		frac = ""
		fraction_a = int(fraction_a)
		while(fraction_a >= 2):
			div = fraction_a % 2
			fraction_a = fraction_a // 2
			if(div==0):
				frac = "0"+frac
			else:
				frac = "1"+frac

		j=0
		k=0
		while(i < self.size-1):
			if(j < self.expsize):
				output = output + exp[j]
				j=j+1
			else:
				if(k < len(frac)):
					output = output+frac[k]
				else:
					output = output + "0"
				k=k+1
			i=i+1

		# print("\n", floatNumber, "\t", str(int(output,2)))

		return str(str(self.size)+"'b"+output)



if __name__=='__main__':
	spn_obj = SPN()
	spn_obj.init('mnist.spn')
	spn_obj.export_sv('mnist.sv')
	# spn_obj.print()