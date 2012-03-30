"""
LLDB AppKit formatters

part of The LLVM Compiler Infrastructure
This file is distributed under the University of Illinois Open Source
License. See LICENSE.TXT for details.
"""
# summary provider for NS(Mutable)IndexSet
import lldb
import ctypes
import objc_runtime
import metrics

statistics = metrics.Metrics()
statistics.add_metric('invalid_isa')
statistics.add_metric('invalid_pointer')
statistics.add_metric('unknown_class')
statistics.add_metric('code_notrun')

# despite the similary to synthetic children providers, these classes are not
# trying to provide anything but the count of values for an NSIndexSet, so they need not
# obey the interface specification for synthetic children providers
class NSIndexSetClass_SummaryProvider:
	def adjust_for_architecture(self):
		pass

	def __init__(self, valobj, params):
		self.valobj = valobj;
		self.sys_params = params
		if not(self.sys_params.types_cache.NSUInteger):
			if self.sys_params.is_64_bit:
				self.sys_params.types_cache.NSUInteger = self.valobj.GetType().GetBasicType(lldb.eBasicTypeUnsignedLong)
			else:
				self.sys_params.types_cache.NSUInteger = self.valobj.GetType().GetBasicType(lldb.eBasicTypeUnsignedInt)
		self.update();

	def update(self):
		self.adjust_for_architecture();

	# NS(Mutable)IndexSet works in one of two modes: when having a compact block of data (e.g. a Range)
	# the count is stored in the set itself, 3 pointers into it
	# otherwise, it will store a pointer to an additional data structure (2 pointers into itself) and this
	# additional structure will contain the count two pointers deep
	# to distinguish the two modes, one reads two pointers deep into the object data: if only the MSB
	# is set, then we are in mode 1, using that area to store flags, otherwise, the read pointer is the
	# location to go look for count in mode 2
	def count(self):
		mode_chooser_vo = self.valobj.CreateChildAtOffset("mode_chooser",
							2*self.sys_params.pointer_size,
							self.sys_params.types_cache.NSUInteger)
		mode_chooser =  mode_chooser_vo.GetValueAsUnsigned(0)
		if self.sys_params.is_64_bit:
			mode_chooser = mode_chooser & 0xFFFFFFFFFFFFFF00
		else:
			mode_chooser = mode_chooser & 0xFFFFFF00
		if mode_chooser == 0:
			mode = 1
		else:
			mode = 2
		if mode == 1:
			count_vo = self.valobj.CreateChildAtOffset("count",
								3*self.sys_params.pointer_size,
								self.sys_params.types_cache.NSUInteger)
		else:
			count_ptr = mode_chooser_vo.GetValueAsUnsigned(0)
			count_vo = self.valobj.CreateValueFromAddress("count",
								count_ptr+2*self.sys_params.pointer_size,
								self.sys_params.types_cache.NSUInteger)
		return count_vo.GetValueAsUnsigned(0)


class NSIndexSetUnknown_SummaryProvider:
	def adjust_for_architecture(self):
		pass

	def __init__(self, valobj, params):
		self.valobj = valobj;
		self.sys_params = params
		self.update();

	def update(self):
		self.adjust_for_architecture();

	def count(self):
		stream = lldb.SBStream()
		self.valobj.GetExpressionPath(stream)
		expr = "(int)[" + stream.GetData() + " count]"
		num_children_vo = self.valobj.CreateValueFromExpression("count",expr)
		if num_children_vo.IsValid():
			return num_children_vo.GetValueAsUnsigned(0)
		return '<variable is not NSIndexSet>'


def GetSummary_Impl(valobj):
	global statistics
	class_data,wrapper = objc_runtime.Utilities.prepare_class_detection(valobj,statistics)
	if wrapper:
		return wrapper
	
	name_string = class_data.class_name()
	if name_string == 'NSIndexSet' or name_string == 'NSMutableIndexSet':
		wrapper = NSIndexSetClass_SummaryProvider(valobj, class_data.sys_params)
		statistics.metric_hit('code_notrun',valobj)
	else:
		wrapper = NSIndexSetUnknown_SummaryProvider(valobj, class_data.sys_params)
		statistics.metric_hit('unknown_class',valobj.GetName() + " seen as " + name_string)
	return wrapper;


def NSIndexSet_SummaryProvider (valobj,dict):
	provider = GetSummary_Impl(valobj);
	if provider != None:
		if isinstance(provider,objc_runtime.SpecialSituation_Description):
			return provider.message()
		try:
			summary = provider.count();
		except:
			summary = None
		if summary == None:
			summary = '<variable is not NSIndexSet>'
		if isinstance(summary, basestring):
			return summary
		else:
			summary = str(summary) + (' objects' if summary != 1 else ' object')
		return summary
	return 'Summary Unavailable'


def __lldb_init_module(debugger,dict):
	debugger.HandleCommand("type summary add -F NSIndexSet.NSIndexSet_SummaryProvider NSIndexSet NSMutableIndexSet")
