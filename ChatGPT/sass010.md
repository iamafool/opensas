hand written or PEGTL?
ChatGPT

Thought about hand-written vs PEGTL for a second
This choice depends largely on your goals, resources, and how comfortable you are with parser formalism and tooling. Here are some considerations to help you decide:

Hand-Written Parser:

Pros:

Full Control: You have complete freedom over every aspect of tokenization and parsing, so you can handle corner cases or unusual constructs precisely as you want.
Simplicity in Concept: A hand-written parser may start very simply, and you can incrementally add features without committing to a particular grammar formalism.
No Additional Dependencies: You don¡¯t rely on external libraries or frameworks, potentially reducing the complexity of setup and build.
Cons:

Time-Consuming: Writing and maintaining a complex grammar by hand is labor-intensive. As the language grows in complexity, your parser code can become unwieldy.
Error-Prone: Without a formal grammar and automated parsing framework, it¡¯s easier to introduce subtle bugs or inconsistencies in how code is parsed.
Difficult Maintenance: As you add more language features, the complexity can grow quickly, making it harder to refactor and keep the parser organized.
PEGTL (Parsing Expression Grammar Template Library):

Pros:

Structured Approach: PEGTL encourages defining a clear, well-structured grammar using PEGs. This can greatly improve maintainability and clarity.
Deterministic Parsing: PEG grammars are inherently deterministic, which simplifies reasoning about the parser¡¯s behavior and reduces ambiguity.
Less Boilerplate: PEGTL handles many of the mechanical aspects of parsing, allowing you to focus on the grammar rules rather than low-level parsing details.
Incremental and Testable Development: It¡¯s often easier to test, debug, and extend a PEGTL-based parser since you¡¯re working with a higher-level declarative grammar.
Cons:

Learning Curve: You need to understand PEGs and PEGTL¡¯s style of grammar definitions. There¡¯s an initial investment in learning how to express grammar rules in PEGTL¡¯s format.
Dependency: Requires integrating an external library, which might be a consideration for some projects.
Recommendation:
If your goal is to create a complex parser that is maintainable, extensible, and less prone to errors as complexity grows, PEGTL is likely the better choice. It provides structure, clarity, and tools out of the box for building and evolving your grammar.

If, on the other hand, you want the absolute freedom to do everything from scratch, have a very specific set of parsing behaviors that don¡¯t fit well into a formal grammar, or enjoy the challenge of building your own parsing infrastructure, a hand-written parser might be more rewarding.

For most practical scenarios¡ªespecially something as large and complex as even a subset of SAS¡ªPEGTL will save you significant time and headaches compared to a completely hand-written approach.