Which one do you recommend for my program?
ChatGPT

Thought about PEGTL vs. Boost.Spirit for a couple of seconds
For a complex project like a SAS interpreter, you¡¯ll likely want a parsing solution that¡¯s maintainable, easier to reason about, and doesn't add unnecessary complexity. PEGTL often fits these criteria better:

Simplicity of Grammar: PEGTL¡¯s approach with Parsing Expression Grammars provides a clearer, more direct mapping between your grammar rules and the resulting parser logic. This can be especially helpful as you tackle the complexity of even a subset of SAS syntax.
Deterministic Parsing: PEGs are inherently deterministic, so you generally avoid the ambiguity and backtracking complexity that can arise with other grammar formalisms. For a large, evolving grammar, this determinism simplifies debugging and maintenance.
Modern, Incremental Development: PEGTL encourages building up your grammar incrementally. You can start with a small subset of SAS features, test them, and then add more rules over time. PEGTL¡¯s style also integrates well with modern C++ practices.
That said, Boost.Spirit is a mature library with a rich ecosystem, and it can work well if you already have experience with it or if you need some of its advanced features. However, given that you¡¯re starting from scratch and aiming to reduce complexity, I¡¯d recommend starting with PEGTL. It generally leads to cleaner, more maintainable code for large, custom language parsers.