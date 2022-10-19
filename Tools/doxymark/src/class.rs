use crate::document::Document;
use crate::element::Element;

use std::io::Write;

struct Function {
    pub name: String,
    pub description: String,
}

impl Function {
    pub fn new() -> Function {
        Function {
            name: String::new(),
            description: String::new(),
        }
    }
}

pub struct Class {
    pub path: String,
    name: String,
    full_name: String,
    description: String,
    functions: Vec<Function>,
}

impl Class {
    pub fn new() -> Self {
        Self {
            path: String::new(),
            name: String::new(),
            full_name: String::new(),
            description: String::new(),
            functions: Vec::<Function>::new(),
        }
    }

    pub fn set_full_name(&mut self, full_name: &str) {
        self.full_name = full_name.to_string();
        self.name = full_name.to_string();

        let split = full_name.split("::");

        if let Some(last) = split.last() {
            self.name = last.to_string();
        }
    }

    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn write(&self, file: &std::fs::File) -> Result<(), std::io::Error> {
        let mut writer = std::io::BufWriter::new(file);

        writeln!(writer, "# {}", self.name)?;

        Ok(())
    }

    pub fn parse(&mut self) -> bool {
        let path = std::path::Path::new(&self.path);
        if !path.exists() {
            return false;
        }

        if let Some(document) = Document::load(&path) {
            if let Some(root) = document.get_element("doxygen") {
                if let Some(def) = root.get_element("compounddef") {
                    self.parse_def(def);
                }
            }
        }

        true
    }

    fn parse_def(&mut self, root: &Element) {

        self.description = Self::get_description(root);

        let sections: Vec<&Element> = root.get_elements("sectiondef");
        for section in &sections {
            if section.get_attribute("kind") == "public-func" {
                let mut functions = self.parse_members(section);
                self.functions.append(&mut functions);
            }
        }
    }

    fn parse_members(&self, root: &Element) -> Vec<Function> {
        let member_defs: Vec<&Element> = root.get_elements("memberdef");

        let mut result = Vec::<Function>::new();
        for member in member_defs {
            if member.get_attribute("kind") == "function" {
                let description = Self::get_description(member);
                if !description.is_empty() {
                    if let Some(name) = member.get_element("name") {
                        let mut function = Function::new();
                        function.name = name.get_inner();
                        function.description = description;
                        result.push(function);
                    }
                }
            }
        }

        result
    }

    fn get_description(element: &Element) -> String {
        let mut result = String::new();

        if let Some(detaileddescription) = element.get_element("detaileddescription") {
            result = Self::parse_description(detaileddescription);
        }

        if result.is_empty() {
            if let Some(briefdescription) = element.get_element("briefdescription") {
                result = Self::parse_description(briefdescription);
            }
        }

        result
    }

    fn parse_description(element: &Element) -> String {
        let mut result = String::new();

        let paras: Vec<&Element> = element.get_elements("para");
        for (index, para) in paras.iter().enumerate() {
            result.push_str(&para.get_inner());

            if index < paras.len() - 1 {
                result.push_str("\n\n");
            }
        }

        result
    }
}
