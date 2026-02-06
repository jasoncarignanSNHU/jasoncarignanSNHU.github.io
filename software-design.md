# Software Design & Engineering  
## Artifact One: Contact Management Application

[See the original and enhanced versions here](ArtifactOne/)

Contents include:
- `ArtifactOne_Original.zip` (Java version from CS 320)
- `ArtifactOne_Enhanced.zip` (Python + Flask implementation)
- Supporting screenshots

---

**Artifact Overview**

The artifact selected for this enhancement is a Contact Management application originally created in CS 320: Software Testing, Automation, and Quality Assurance as a Java-based project. The original version consisted of a domain model, a service layer, and automated unit tests designed to validate contact information such as names, phone numbers, and addresses. The application did not include a user interface and instead functioned as a validation and service harness capable of supporting a future system requiring reliable contact data handling.

For CS 499, this artifact was enhanced by fully porting the application from Java to Python, restructuring it into a professional package format, recreating the automated test suite using pytest, and integrating a lightweight Flask web interface that allows users to add and delete contacts through a browser. These enhancements transformed the project from a test-focused backend into a functional, user-facing prototype while preserving the original validation and business rules. As a result, the artifact now reflects a realistic software system rather than a standalone academic exercise, making it well suited for inclusion in my ePortfolio.

---

**Engineering Decisions and Technical Depth**

Enhancing this artifact required careful analysis of the original Java code to ensure that all business rules were preserved during the conversion to Python. A specific challenge during this migration involved translating Java’s strongly typed class structure and explicit constructor validation into Python’s more flexible object model. Because Python does not enforce type constraints at compile time, validation logic that was implicitly protected by Java’s typing system had to be made more explicit through runtime checks and controlled object creation patterns. This required redesigning portions of the initialization logic to ensure consistent enforcement of field length constraints and null-handling behavior.

Another major challenge was adapting the service layer to support live form input rather than direct object creation. This change required additional validation and error handling to ensure that the system remained stable when exposed to user-driven workflows. Rewriting the test suite using pytest reinforced the importance of generic test data and comprehensive boundary testing, while integrating Flask demonstrated how backend logic must remain robust when paired with a user-facing interface.

Additional defensive measures were implemented to ensure:
- Strict field length enforcement  
- Duplicate contact ID detection  
- Controlled exception handling  
- Prevention of malformed or incomplete input  

Flask was intentionally selected as the web framework because it provided a lightweight solution that allowed backend logic to remain cleanly separated from presentation concerns. While more complex frameworks offer scalability features, Flask was the most appropriate choice for demonstrating how a validated service layer can be exposed through a simple interface without introducing architectural complexity. This design trade-off preserved clarity while still demonstrating real-world applicability.

---

**Validation and Testing**

The completed application is verified through both automated testing and live execution. The test suite validates correct contact creation, rejection of invalid fields, duplicate ID handling, and update and delete behaviors, and all tests pass successfully.

![PyCharm Test Coverage](ArtifactOne/PyCharm%20Test%20Coverage.png)

The image above shows full test coverage in PyCharm, demonstrating that validation logic remains comprehensive and reliable.

---

**User Interface Integration**

The Flask interface demonstrates how the same validated logic is reused through a browser-based form, confirming that the service layer remains consistent even when accessed through a web application.

![Flask UI](ArtifactOne/Flask%20UI.png)

This interface confirms that backend rules remain stable even when exposed to user-driven workflows, reinforcing the importance of layered system design.

---

**Documentation and Reproducibility**

![Folder and README](ArtifactOne/Folder%20%26%20README.png)

The README included in the enhanced artifact provides setup instructions, dependency documentation, and execution steps so that another developer can download and run the project without ambiguity. This reflects professional communication standards and aligns with industry expectations for collaborative development.

---

**Alignment With Program Outcomes**

This enhancement demonstrates measurable progress toward all five Computer Science program outcomes:

1. **Outcome 1: Employ strategies for building collaborative environments that enable diverse audiences to support organizational decision making.** The modular architecture, clear separation of concerns (domain model, service layer, testing suite, and web interface), and structured documentation support collaborative development and allow technical and nontechnical stakeholders to understand system behavior.

2. **Outcome 2: Design, develop, and deliver professional-quality communications adapted to specific audiences and contexts.** The project includes structured packaging, meaningful naming conventions, inline documentation, a comprehensive README file, and a browser-based interface, ensuring clarity for developers and usability for end users.

3. **Outcome 3: Design and evaluate computing solutions using algorithmic principles and computer science practices.** Strict validation rules, boundary condition testing, duplicate detection logic, and comprehensive automated testing demonstrate systematic evaluation of computing solutions and adherence to algorithmic rigor.

4. **Outcome 4: Use well-founded and innovative techniques, skills, and tools to implement computing solutions that deliver value.** The migration from Java to Python, the use of pytest for automated testing, and the integration of a Flask-based web interface reflect deliberate tool selection aligned with modern industry practices.

5. **Outcome 5: Develop a security mindset that anticipates vulnerabilities and mitigates risks.** Defensive programming practices, including controlled object creation, strict input validation, exception handling, and duplicate ID enforcement to reduce system vulnerability and promote data integrity.


---

**Reflection**

Overall, this enhancement strengthened my understanding of software structure, validation, automated testing, and real-world usability. It reinforced how a well-designed service layer can act as a stable foundation for future systems and how existing code can be modernized through thoughtful design and tool selection. By transforming this artifact into a Python-based application with a working interface and full test coverage, I demonstrated how professional software engineering practices can extend the value of an existing system while aligning it with modern development standards.
