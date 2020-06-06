from spn import SPN
from psdd import PSDD

if __name__=='__main__':
	spn_filename = "example_03.spn"
	psdd_filename = "example_03.psdd"

	psdd_obj = PSDD(psdd_filename)
	spn_obj = SPN()
	spn_obj.node_list = psdd_obj.export_spn()
	spn_obj.print2file(spn_filename)
