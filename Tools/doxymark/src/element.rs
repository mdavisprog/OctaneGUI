use std::collections::HashMap;

pub struct Element {
    pub name: String,
    pub values: Vec<String>,
    pub attributes: HashMap<String, String>,
    pub elements: Vec::<Element>,
}

impl Element {
    pub fn new() -> Self {
        Self {
            name: String::new(),
            values: Vec::<String>::new(),
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
            result = self.values.concat();
        } else {
            let mut i = 0;

            if self.values.len() >= self.elements.len() {
                while i < self.values.len() {
                    result.push_str(&self.values[i]);
                    
                    if i < self.elements.len() {
                        result.push_str(&self.elements[i].get_inner());
                    }
    
                    i += 1;
                }
            } else {
                while i < self.elements.len() {
                    result.push_str(&self.elements[i].get_inner());

                    if i < self.values.len() {
                        result.push_str(&self.values[i]);
                    }

                    i += 1;
                }
            }
        }

        result
    }

    fn print(&self, f: &mut std::fmt::Formatter, indent: usize) {
        match writeln!(f, "{:tab$}{} => {}", "", self.name, self.values.concat(), tab=indent) {
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
