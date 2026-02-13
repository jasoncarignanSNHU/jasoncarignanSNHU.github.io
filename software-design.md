# Software Design & Engineering  
[Home](https://jasoncarignansnhu.github.io/)

## Artifact One: Contact Management Application

[See the original and enhanced versions here](https://github.com/jasoncarignanSNHU/jasoncarignanSNHU.github.io/tree/ePortfolio/ArtifactOne)

Contents include:
- `ArtifactOne_Original.zip` (Java version from CS 320)
- `ArtifactOne_Enhanced.zip` (Python + Flask implementation)
- Supporting screenshots

---

### Artifact Overview

The artifact selected for this enhancement is a Contact Management application originally developed in **CS 320: Software Testing, Automation, and Quality Assurance**. The original Java-based implementation included a domain model, a service layer, and automated unit tests that validated contact information such as names, phone numbers, and addresses. It functioned as a backend validation system without a user interface.

For CS 499, this artifact was selected because it provided a strong opportunity to demonstrate software engineering growth through language migration, structural redesign, and user-facing integration. At a high level, the enhancement improves the system in three meaningful ways: it modernizes the implementation using Python, expands the architecture into a complete web-accessible application, and strengthens validation and defensive programming practices to reflect real-world development standards.

The enhanced version preserves the original validation logic while transforming the project from a test-focused academic exercise into a functional, user-facing prototype.

---

### Architectural Redesign and Language Migration

The first major enhancement involved fully porting the application from Java to Python. This required careful analysis of the original object-oriented structure to ensure that all validation rules and service behaviors were preserved.

A key challenge during this migration was adapting Java’s strongly typed class design to Python’s more flexible object model. Because Python does not enforce compile-time type checking, validation logic that was implicitly protected in Java had to be explicitly enforced at runtime. Constructor validation, field constraints, and null-handling logic were redesigned to ensure consistent enforcement of:

- Required fields  
- Maximum length constraints  
- Phone number formatting rules  
- Controlled object creation  

The project was reorganized into a professional Python package structure with clearly separated modules for domain objects, service logic, and exceptions. This restructuring improved maintainability and readability while aligning with modern Python development conventions.

---

### Testing Strategy and Reliability

The original JUnit test suite was fully recreated using `pytest`, preserving the validation coverage while modernizing the testing framework. The enhanced test suite evaluates:

- Valid contact creation  
- Invalid field rejection  
- Boundary condition handling  
- Duplicate contact ID detection  
- Update and delete operations  

All tests pass successfully, and full coverage was verified during development.

![PyCharm Test Coverage](ArtifactOne/PyCharm%20Test%20Coverage.png)

This verification confirms that the migration preserved functional integrity while strengthening reliability.

---

### User Interface Integration

To extend the artifact beyond backend validation, a lightweight Flask interface was introduced. This addition exposes the validated service layer through a browser-based form, allowing users to add and delete contacts interactively.

![Flask UI](ArtifactOne/Flask%20UI.png)

Flask was intentionally selected because it provides a minimal framework that keeps backend logic cleanly separated from presentation concerns. While larger frameworks offer scalability features, Flask allowed the focus to remain on architectural clarity and service-layer integrity. This design decision demonstrates deliberate tool selection aligned with project scope and maintainability.

Adapting the service layer to accept live form input required additional error handling and input validation safeguards. The system now defensively handles malformed or incomplete user submissions while preserving the original business rules.

---

### Documentation and Reproducibility

![Folder and README](ArtifactOne/Folder%20%26%20README.png)

The enhanced artifact includes a comprehensive README file that provides setup instructions, dependency requirements, and execution steps. This ensures that another developer can clone, install, and run the project without ambiguity.

This emphasis on reproducibility reflects professional communication standards and supports collaborative development environments.

---

### Alignment With Program Outcomes

This enhancement demonstrates measurable progress toward the Computer Science program outcomes:

1. **Outcome 1: Employ strategies for building collaborative environments that enable diverse audiences to support organizational decision making.**  
   The modular architecture, structured package design, documentation, and separation of concerns support maintainability and collaborative review.

2. **Outcome 2: Design, develop, and deliver professional-quality communications adapted to specific audiences and contexts.**  
   Clear documentation, organized code structure, meaningful naming conventions, and a browser-based interface communicate system behavior effectively to both technical and nontechnical audiences.

3. **Outcome 3: Design and evaluate computing solutions using algorithmic principles and computer science practices.**  
   Consistent validation logic, boundary testing, duplicate detection, and structured service-layer design demonstrate systematic evaluation of computing solutions.

4. **Outcome 4: Use well-founded and innovative techniques, skills, and tools to implement computing solutions that deliver value.**  
   The migration from Java to Python, integration of `pytest`, and implementation of a Flask-based interface reflect modern tool selection aligned with industry practices.

5. **Outcome 5: Develop a security mindset that anticipates vulnerabilities and mitigates risks.**  
   Strict runtime validation, controlled object creation, duplicate prevention, and structured exception handling reduce vulnerability and protect system integrity.

---

### Reflection

Enhancing this artifact required balancing architectural clarity, validation rigor, usability, and maintainability. Migrating from Java to Python strengthened my understanding of language-level differences in type safety and validation enforcement. Integrating Flask demonstrated how backend systems must remain stable when exposed to user-driven workflows.

Overall, this enhancement transformed a validation-focused academic project into a complete, structured application that reflects professional software engineering standards. It demonstrates language migration, modular design, automated testing, user interface integration, and defensive programming in a cohesive system aligned with real-world development practices.
