use std::collections::HashMap;

pub struct Element {
    pub name: String,
    pub value: String,
    pub attributes: HashMap<String, String>,
    pub elements: Vec::<Element>,
}

impl Element {
    pub fn new() -> Self {
        Self {
            name: String::new(),
            value: String::new(),
            attributes: HashMap::<String, String>::new(),
            elements: Vec::<Element>::new(),
        }
    }

    pub fn get_elements(&self, name: &str) -> Vec<&Element> {
        let mut result = Vec::<&Element>::new();

        for element in &self.elements {
            if element.name == name {
                result.push(element);
            }
        }

        result
    }

    pub fn get_element(&self, name: &str) -> Option<&Element> {
        for element in &self.elements {
            if element.name == name {
                return Some(element);
            }
        }

        None
    }

    pub fn get_attribute(&self, name: &str) -> &str {
        if self.attributes.contains_key(name) {
            return &self.attributes[name];
        }

        ""
    }

    /// Retrieve all inner values within an element. This will include values within
    /// sub-elements. An example may be a description with links to external references.
    pub fn get_inner(&self) -> String {
        let mut result = String::new();

        if self.elements.is_empty() {
            result = self.value.clone();
        } else {
            for element in &self.elements {
                result += &element.get_inner();
            }
        }

        result
    }

    fn print(&self, f: &mut std::fmt::Formatter, indent: usize) {
        match writeln!(f, "{:tab$}{} => '{}'", "", self.name, self.value, tab=indent) {
            Err(error) => {
                panic!("Error writing to formatter: {}", error);
            }
            _ => {}
        }

        for element in &self.elements {
            element.print(f, indent + 2);
        }
    }
}

impl std::fmt::Display for Element {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        self.print(f, 0);
        Ok(())
    }
}
