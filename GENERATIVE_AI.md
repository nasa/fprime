# F´ Generative AI Usage Guidelines

We're excited about the potential of generative AI to help make [F´](https://github.com/nasa/fprime) development more productive, enjoyable, and accessible! Whether you're using AI to write code, improve documentation, or learn about complex systems, we welcome the thoughtful use of these powerful tools in your F´ contributions.

This guide shares our community's approach to using generative AI effectively and responsibly. You'll find practical tips, best practices, and simple guidelines to help you get the most out of AI tools while maintaining the quality standards that make F´ great.

## Our Position on Generative AI

F´ embraces technological advancement and innovation. Generative AI tools can assist with:

- Code generation and refactoring
- Documentation creation and improvement  
- Test case development
- Debugging assistance
- Design pattern suggestions
- Learning and understanding our codebases

However, the use of generative AI must align with our commitment to high technical standards, quality, and the collaborative nature of open source development.

## Disclosure

To maintain transparency and enable effective code review, contributors must disclose generative AI usage in pull requests:

### Pull Request Submission: Contributors

1. **Fill-In 'y' to the "AI Used (y/n)" table entry** in the pull request template
2. **Provide details in the "AI Usage" section** describing how generative AI was utilized

### What to Disclose

Include information about:

- **Type of assistance**: Code generation, documentation, debugging, testing, refactoring, etc.
- **Scope of usage**: Which files, functions, or sections were AI-assisted
- **Tool(s) used**: Name of the AI system(s) employed (e.g., GitHub Copilot, ChatGPT, etc.)
- **Level of modification**: Whether AI-generated content was used as-is, modified, or used as inspiration

### Example Disclosure

```
Used GitHub Copilot for:
- Generated unit test templates for TestFlightController.cpp (modified for F Prime conventions)
- Documentation improvements in README.md (reviewed and edited for accuracy)
```
## Limitations and Considerations

### What AI Cannot Replace

- **Domain expertise** in flight software and embedded systems
- **Understanding of F Prime architecture** and design patterns
- **Critical thinking** about system requirements and constraints
- **Human judgment** on safety-critical decisions
- **Community collaboration** and peer review processes

## Best Practices

### Providing Guidelines to AI Tools

When working with generative AI, provide clear rules and context to improve code quality and consistency. For Example:

- **Reference F´ Style Guidelines**: Include the [F´ Style Guidelines](https://github.com/nasa/fprime/wiki/F%C2%B4-Style-Guidelines) in your prompts
- **Specify API documentation**: Direct AI to use the [F´ C++ API documentation](https://nasa.github.io/fprime/v3.4.1/UsersGuide/api/c++/html/md_docs_2doc-index.html) for code generation
- **Enforce coding standards**: Instruct AI to avoid "magic numbers" and use descriptive variable names or comments
- **Provide project context**: Share relevant F´ architectural patterns and component structures

### Quality and Responsibility

- **Review all AI-generated code** thoroughly before submission
- **Verify necessity and relevance** - Remove verbose or unnecessary AI-generated content
- **Be concise** - Edit AI output to be clear and to-the-point
- **Ensure compliance** with F Prime coding standards and style guidelines
- **Verify correctness** and test all AI-assisted implementations
- **Maintain authorship responsibility** - you are accountable for all submitted code regardless of its origin

### Security

- **Avoid sharing sensitive information** with AI tools (proprietary project designs, mission-specific details, etc.)
- **Be cautious with external dependencies** suggested by AI tools
- **Validate security implications** of AI-generated code, especially for flight software

### Code Review Considerations

- **Provide context** to reviewers about AI usage to enable informed evaluation
- **Be prepared to explain** AI-generated logic and design decisions
- **Accept feedback gracefully** - AI-generated code is not exempt from revision requests
- **Document complex AI-assisted algorithms** clearly for future maintainers

## Reviewer Guidelines

When reviewing pull requests with disclosed AI usage:

### Focus Areas

- **Logic correctness** - Verify AI-generated algorithms work as intended
- **Code quality** - Ensure adherence to F Prime standards regardless of origin
- **Integration** - Check that AI-generated code integrates properly with existing systems
- **Documentation** - Verify that AI-assisted documentation is accurate and complete
- **Testing** - Ensure adequate test coverage for AI-generated functionality

### Evaluation Approach

- **No bias against AI usage** - Evaluate code on its merits, not its origin
- **Enhanced scrutiny where appropriate** - Complex AI-generated logic may warrant additional review
- **Educational opportunity** - Help contributors improve their use of AI tools through feedback


### Ongoing Evaluation

As generative AI technology evolves, this policy will be updated to reflect:

- New capabilities and tools
- Lessons learned from community usage
- Industry best practices
- Community feedback and needs

## Getting Help

If you have questions about appropriate AI usage or need guidance on disclosure:

- Open a [Discussion](https://github.com/nasa/fprime/discussions) for community input
- Contact the Community Managers for specific guidance

## Conclusion

F Prime's approach to generative AI balances innovation with responsibility. By encouraging transparent usage and maintaining our quality standards, we can harness the benefits of AI tools while preserving the collaborative, high-quality nature of our open source project.

Remember: AI is a tool to enhance human capability, not replace human responsibility. All contributors remain fully accountable for their contributions to F Prime, regardless of the tools used to create them.
