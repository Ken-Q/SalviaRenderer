#ifndef SASL_SEMANTIC_SEMANTICS_H
#define SASL_SEMANTIC_SEMANTICS_H

#include <sasl/include/semantic/semantic_forward.h>

#include <sasl/enums/literal_classifications.h>
#include <sasl/enums/builtin_types.h>
#include <eflib/include/utility/shared_declaration.h>
#include <eflib/include/platform/typedefs.h>

#include <eflib/include/platform/boost_begin.h>
#include <boost/shared_ptr.hpp>
#include <eflib/include/platform/boost_end.h>

#include <string>
#include <vector>

namespace salviar
{
	class semantic_value;
}

namespace sasl
{
	namespace syntax_tree
	{
		struct tynode;
		struct labeled_statement;
		EFLIB_DECLARE_STRUCT_SHARED_PTR(node);
		EFLIB_DECLARE_STRUCT_SHARED_PTR(program);
	}
	namespace common
	{
		EFLIB_DECLARE_CLASS_SHARED_PTR(diag_chat);
	}
}

BEGIN_NS_SASL_SEMANTIC();

int32_t swizzle_field_name_to_id( char ch );
int32_t encode_swizzle( char _1st, char _2nd = 0, char _3rd = 0, char _4th = 0 );
int32_t encode_swizzle( int& dest_size, int& min_src_size, char const* masks );
int32_t encode_sized_swizzle( int size );

class node_semantic;
class pety_t;
EFLIB_DECLARE_CLASS_SHARED_PTR(symbol);
EFLIB_DECLARE_CLASS_SHARED_PTR(module_semantic);

class module_semantic
{
public:
	static module_semantic_ptr create();

	virtual ~module_semantic(){}
	
	virtual symbol*							root_symbol() const = 0;
	virtual sasl::syntax_tree::program_ptr	get_program() const = 0;
	virtual	void							set_program(sasl::syntax_tree::program_ptr const&) = 0;

	virtual pety_t*							pety() const = 0;
	virtual sasl::common::diag_chat_ptr		diags() const = 0;

	virtual std::vector<symbol*> const&	global_vars() const = 0;
	virtual std::vector<symbol*>&		global_vars() = 0;

	virtual std::vector<symbol*> const&	functions() const = 0;
	virtual std::vector<symbol*>&		functions() = 0;
	
	virtual std::vector<symbol*> const&	intrinsics() const = 0;
	virtual std::vector<symbol*>&		intrinsics() = 0;

	template <typename T> node_semantic* get_semantic( boost::shared_ptr<T> const& v )
	{
		return get_semantic( v.get() );
	}

	template <typename T> node_semantic* get_or_create_semantic( boost::shared_ptr<T> const& v )
	{
		return get_or_create_semantic(v.get() );
	}

	template <typename T> node_semantic* create_semantic(boost::shared_ptr<T> const& v)
	{
		return create_semantic( v.get() );
	}

	virtual node_semantic* get_semantic(sasl::syntax_tree::node const*) const = 0;
	virtual node_semantic* create_semantic(sasl::syntax_tree::node const*) = 0;
	virtual node_semantic* get_or_create_semantic(sasl::syntax_tree::node const*) = 0;
	
	virtual void	hold_node(sasl::syntax_tree::node_ptr const& ) = 0;
	virtual symbol* get_symbol(sasl::syntax_tree::node*) const = 0;
	virtual symbol* alloc_symbol() = 0; ///< Only called by symbol.
	virtual void	link_symbol(sasl::syntax_tree::node*, symbol*) = 0;
};

class node_semantic
{
public:
	typedef std::vector<
		boost::weak_ptr<sasl::syntax_tree::labeled_statement>
	>		labeled_statement_array;

	~node_semantic();

	node_semantic(node_semantic const&);
	node_semantic& operator = (node_semantic const&);

	// Read functions
public:
	// General
	module_semantic*
			owner() const { return owner_; }
	sasl::syntax_tree::node*
			associated_node() const { return assoc_node_; }
	symbol*	associated_symbol() const { return assoc_symbol_; }

	// Type
	int		tid() const	{ return tid_; }
	builtin_types
			value_builtin_type() const;
	sasl::syntax_tree::tynode*
			ty_proto() const;

	// Constant
	int64_t const_signed() const { return signed_constant_; }
	uint64_t
			const_unsigned() const { return unsigned_constant_; }
	double	const_double() const { return double_constant_; }
	std::string
			const_string() const;

	// Expression and variable
	salviar::semantic_value*
			semantic_value() const { return semantic_value_; }
	salviar::semantic_value const&
			semantic_value_ref() const;
	int		member_index() const { return member_index_; }
	int32_t	swizzle() const {return swizzle_code_; }
	bool	is_reference() const { return is_reference_; }
	bool	is_function_pointer() const { return is_function_pointer_; }

	// Function and intrinsic
	std::string const&
			function_name() const;
	symbol*	overloaded_function() const { return overloaded_function_; }
	bool	is_intrinsic() const { return is_intrinsic_; }
	bool	is_external() const { return is_external_; }
	bool	msc_compatible() const { return msc_compatible_; }
	bool	is_invoked() const { return is_invoked_; }
	bool	partial_execution() const { return partial_execution_; }
	bool	is_constructor() const { return is_constructor_; }

	// Statement
	labeled_statement_array const&
			labeled_statements() const;
	//sasl::syntax_tree::node*
	//		parent_block() const { return parent_block_; }
	bool	has_loop() const { return has_loop_; }
	
	// Write functions
public:
	// General
	void owner(module_semantic* v) { owner_ = v; }

	void associated_node(sasl::syntax_tree::node* v) { assoc_node_ = v; }
	void associated_symbol(symbol* v) { assoc_symbol_ = v; }

	// Type
	void tid(int v);
	void internal_tid(int v, sasl::syntax_tree::tynode* proto); /// Only used by pety.
	void ty_proto(sasl::syntax_tree::tynode* ty, symbol* scope);

	// Constant
	void const_value(std::string const& lit, literal_classifications lit_class);

	void const_value(int64_t v) { signed_constant_ = v; }
	void const_value(uint64_t v) { unsigned_constant_ = v; }
	void const_value(std::string const& v);
	void const_value(double v) { double_constant_ = v; }

	// Expression and variable
	void semantic_value(salviar::semantic_value const& v);

	void member_index(int v) { member_index_ = v; }
	void swizzle(int32_t v) { swizzle_code_ = v; }
	void is_reference(bool v) { is_reference_ = v; }
	void is_function_pointer(bool v) { is_function_pointer_ = v; }

	// Function and intrinsic
	void function_name(std::string const& v);
	void overloaded_function(symbol* v) { overloaded_function_ = v; }
	void is_intrinsic(bool v) { is_intrinsic_ = v; }
	void is_external(bool v) { is_external_ = v; }
	void msc_compatible(bool v) { msc_compatible_ = v; }
	void is_invoked(bool v) { is_invoked_ = v; }
	void partial_execution(bool v) { partial_execution_ = v; }
	void is_constructor(bool v) { is_constructor_ = v; }

	// Statement
	labeled_statement_array& labeled_statements();
	//void parent_block(sasl::syntax_tree::node* v) { parent_block_ = v; }
	void has_loop(bool v) { has_loop_ = v; }

private:
	sasl::syntax_tree::node* assoc_node_;
	module_semantic*	owner_;
	symbol*				assoc_symbol_;
	
	// Type
	sasl::syntax_tree::tynode*	
			proto_type_;
	int 	tid_;
	
	// Constant
	int64_t			signed_constant_;
	uint64_t		unsigned_constant_;
	std::string*	string_constant_;
	double			double_constant_;
	
	// Expression and variable
	salviar::semantic_value* semantic_value_;
	int		member_index_;
	int32_t	swizzle_code_;
	bool	is_reference_;
	bool	is_function_pointer_;
	
	// Function and intrinsic
	std::string* function_name_;
	symbol*	overloaded_function_;
	bool	is_intrinsic_;
	bool	is_invoked_;
	bool	msc_compatible_;
	bool	is_external_;
	bool	partial_execution_;
	bool	is_constructor_;
	
	// Statement
	labeled_statement_array* labeled_statements_;
	//sasl::syntax_tree::node* parent_block_;
	bool	has_loop_;
};

END_NS_SASL_SEMANTIC();

#endif